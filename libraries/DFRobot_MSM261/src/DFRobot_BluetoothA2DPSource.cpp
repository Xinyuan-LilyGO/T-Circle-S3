/*!
 * @file DFRobot_BluetoothA2DPSource.cpp
 * @brief 蓝牙麦克风的实现
 * @copyright	Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version V1.0
 * @date 2022-2-18
 * @url https://github.com/DFRobot/DFRobot_MSM261
 */
#include "DFRobot_BluetoothA2DPSource.h"

#define BT_APP_SIG_WORK_DISPATCH            (0x01)
#define BT_APP_SIG_WORK_DISPATCH            (0x01)

#define APP_RC_CT_TL_RN_VOLUME_CHANGE       (1)
#define BT_APP_HEART_BEAT_EVT               (0xff00)
#define 	APP_AV_STATE_IDLE                 (0)
#define 	APP_AV_STATE_DISCOVERING          (1)
#define 	APP_AV_STATE_DISCOVERED           (2)
#define 	APP_AV_STATE_UNCONNECTED          (3)
#define 	APP_AV_STATE_CONNECTING           (4)


DFRobot_BluetoothA2DPSource *_selfBluetoothA2DPSource;
void DFRobot_BluetoothA2DPSource::callBtAppGapCb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param){
  if(_selfBluetoothA2DPSource) _selfBluetoothA2DPSource->btAppGapCb(event,param);
}
void DFRobot_BluetoothA2DPSource::callBtAppRcCtCb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param){
  if(_selfBluetoothA2DPSource) _selfBluetoothA2DPSource->btAppRcCtCb(event,param);
}
void DFRobot_BluetoothA2DPSource::callBtAppAvStateConnecting(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param){
  if(_selfBluetoothA2DPSource) _selfBluetoothA2DPSource->btAppAvStateConnecting(event,param);
}

DFRobot_BluetoothA2DPSource::DFRobot_BluetoothA2DPSource(void){
  _selfBluetoothA2DPSource = this;
  sA2DState = APP_AV_STATE_IDLE;

}


bool DFRobot_BluetoothA2DPSource::begin(const char* name, esp_a2d_source_data_cb_t callback, uint8_t volume)
{
	_bleName = name;
  _volume = volume;
	const char *devName = "ESP_A2DP_SRC";
  if (!btStarted() && !btStart()){
    DBG("Initialize controller failed");
    return false;
  }
  esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
  if (bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED){
    if (esp_bluedroid_init()) {
      DBG("Initialize bluedroid failed !");
      return false;
    }
  }
  if (bt_state != ESP_BLUEDROID_STATUS_ENABLED){
    if (esp_bluedroid_enable()) {
      DBG("Enable bluedroid failed !");
      return false;
    }
  }
	if(esp_bt_dev_set_device_name(devName)){
		DBG("set device name failed !");
		return false;
	}
	if(esp_bt_gap_register_callback(callBtAppGapCb)){
		DBG("set gap register failed!");
		return false;
	}
	if(esp_avrc_ct_init()){
		DBG("Initialize avrc failed!");
		return false;
	}
  if(esp_avrc_ct_register_callback(callBtAppRcCtCb)){
    return false;
  }
	esp_a2d_register_callback(callBtAppAvStateConnecting);
  esp_a2d_source_register_data_callback(callback);
  esp_a2d_source_init();
	if (esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE)){
    DBG("Set discoverability and connectability mode for legacy bluetooth failed !");
    return false;
  }
	sA2DState = APP_AV_STATE_DISCOVERING;
  esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
	return true;

}
void DFRobot_BluetoothA2DPSource::btAppGapCb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
	switch (event) {
    case ESP_BT_GAP_DISC_RES_EVT: {
      filterInquiryScanResult(param);
      break;
    }
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: {
      if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
        if (sA2DState == APP_AV_STATE_DISCOVERED) {
          sA2DState = APP_AV_STATE_CONNECTING;
					Serial.println("Device discovery stopped.");
					Serial.print("a2dp connecting to peer: ");
					Serial.println((char*)sPeerBdname);
          esp_a2d_source_connect(sPeerBda);
        } else {
					Serial.print("Device discovery failed, continue to discover...");
          esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
        }
      } else if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STARTED) {
				Serial.print("Discovery started.");
      }
      break;
    }
    case ESP_BT_GAP_RMT_SRVCS_EVT:
    case ESP_BT_GAP_RMT_SRVC_REC_EVT:
      break;
    case ESP_BT_GAP_AUTH_CMPL_EVT: 
      break;
    case ESP_BT_GAP_PIN_REQ_EVT: 
      break;
    case ESP_BT_GAP_CFM_REQ_EVT:
      break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
      break;
    case ESP_BT_GAP_KEY_REQ_EVT:
      break;
    default: {
    //   ESP_LOGI(BT_AV_TAG, "event: %d", event);
      break;
    }
  }
  return;
}
void DFRobot_BluetoothA2DPSource::filterInquiryScanResult(esp_bt_gap_cb_param_t *param)
{
	char bda_str[18];
  uint32_t cod = 0;
  int32_t rssi = -129; /* invalid value */
  uint8_t *eir = NULL;
  esp_bt_gap_dev_prop_t *p;

	Serial.print("Scanned device:");
	Serial.println(bda2str(param->disc_res.bda, bda_str, 18));
  for (int i = 0; i < param->disc_res.num_prop; i++) {
    p = param->disc_res.prop + i;
    switch (p->type) {
    	case ESP_BT_GAP_DEV_PROP_COD:
        cod = *(uint32_t *)(p->val);
				Serial.print("--Class of Device: 0x");
				Serial.println(cod);
        break;
      case ESP_BT_GAP_DEV_PROP_RSSI:
        rssi = *(int8_t *)(p->val);
				Serial.print("--RSSI:");
				Serial.println(rssi);
        break;
      case ESP_BT_GAP_DEV_PROP_EIR:
        eir = (uint8_t *)(p->val);
        break;
      case ESP_BT_GAP_DEV_PROP_BDNAME:
      default:
        break;
    }
  }

    /* search for device with MAJOR service class as "rendering" in COD */
  if (!esp_bt_gap_is_valid_cod(cod) ||
    !(esp_bt_gap_get_cod_srvc(cod) & ESP_BT_COD_SRVC_RENDERING)) {
    return;
  }

  if (eir) {
    getNameFromEir(eir, sPeerBdname, NULL);
    if (strcmp((char *)sPeerBdname, _bleName) != 0) {
      return;
    }
		Serial.print("Found a target device, address ");
		Serial.print(bda_str);
		Serial.print(",name");
		Serial.println((char*)sPeerBdname);
    sA2DState = APP_AV_STATE_DISCOVERED;
    memcpy(sPeerBda, param->disc_res.bda, ESP_BD_ADDR_LEN);
		Serial.println("Cancel device discovery ...");
    esp_bt_gap_cancel_discovery();
  }
}
bool DFRobot_BluetoothA2DPSource::getNameFromEir(uint8_t *eir, uint8_t *bdname, uint8_t *bdname_len)
{
  uint8_t *rmt_bdname = NULL;
  uint8_t rmt_bdname_len = 0;
  if (!eir) {
    return false;
  }
  rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
  if (!rmt_bdname) {
    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
  }
  if (rmt_bdname) {
    if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
      rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
    }
    if (bdname) {
      memcpy(bdname, rmt_bdname, rmt_bdname_len);
      bdname[rmt_bdname_len] = '\0';
    }
    if (bdname_len) {
      *bdname_len = rmt_bdname_len;
    }
    return true;
  }
    return false;
}
char *DFRobot_BluetoothA2DPSource::bda2str(esp_bd_addr_t bda, char *str, size_t size)
{
  if (bda == NULL || str == NULL || size < 18) {
    return NULL;
  }
  uint8_t *p = bda;
  sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
  return str;
}
void DFRobot_BluetoothA2DPSource::btAppRcCtCb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
	esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(param);
	DBG(event);
  switch (event) {
    case ESP_AVRC_CT_METADATA_RSP_EVT:
			break;
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
			//uint8_t *bda = rc->conn_stat.remote_bda;
      if (rc->conn_stat.connected) {
        // get remote supported event_ids of peer AVRCP Target
        esp_avrc_ct_send_get_rn_capabilities_cmd(APP_RC_CT_TL_GET_CAPS);
      } else {
        // clear peer notification capability record
        s_avrc_peer_rn_cap.bits = 0;
      }
      break;
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
			break;
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
			break;
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
			break;
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT:
			esp_avrc_ct_send_set_absolute_volume_cmd(APP_RC_CT_TL_RN_VOLUME_CHANGE, _volume);
			btAvVolumeChanged();
			break;
    case ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: {
			esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
			DBG("set volume event");
      break;
    }
    default:
    //   ESP_LOGE(BT_RC_CT_TAG, "Invalid AVRC event: %d", event);
      break;
    }
}
void DFRobot_BluetoothA2DPSource::btAppAvStateConnecting(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
  esp_a2d_cb_param_t *a2d = NULL;
	DBG(event);
  switch (event) {
  case ESP_A2D_CONNECTION_STATE_EVT: {
    a2d = (esp_a2d_cb_param_t *)(param);
    if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
			Serial.println("a2dp connected");
      esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
    } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      sA2DState =  APP_AV_STATE_UNCONNECTED;
			Serial.println("a2dp disconnected");
			esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    }
    break;
    }
    case ESP_A2D_AUDIO_STATE_EVT:
      break;
    case ESP_A2D_AUDIO_CFG_EVT:
			break;
    case ESP_A2D_MEDIA_CTRL_ACK_EVT:
			a2d = (esp_a2d_cb_param_t *)(param);
			if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY &&
        a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
				Serial.println("a2dp media ready, stop ...");
        esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_STOP);		
      }else if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_STOP &&
        a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
				Serial.println("a2dp media ready, starting ...");
				esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START);			
      } if (a2d->media_ctrl_stat.cmd == ESP_A2D_MEDIA_CTRL_START &&
        a2d->media_ctrl_stat.status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
				Serial.println("a2dp media start successfully.");
      }else{
				Serial.print(".");
			}
      break;
    case BT_APP_HEART_BEAT_EVT:
      break;
    default:
    //   ESP_LOGE(BT_AV_TAG, "%s unhandled evt %d", __func__, event);
      break;
  }
}
void DFRobot_BluetoothA2DPSource::btAvVolumeChanged(void)
{
  if (esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_TEST, &s_avrc_peer_rn_cap,
                                           ESP_AVRC_RN_VOLUME_CHANGE)) {
    esp_avrc_ct_send_register_notification_cmd(APP_RC_CT_TL_RN_VOLUME_CHANGE, ESP_AVRC_RN_VOLUME_CHANGE, 0);
  }
}
