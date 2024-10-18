#include "App.hpp"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "git_version.h"
#include "kws_event_task.h"
#include "kws_task.h"

#define TITLE      "VoiceRelay"
#define HEADER_STR TITLE " " TOSTRING(MAJOR_VERSION) "." TOSTRING(MINOR_VERSION)

static constexpr char TAG[] = TITLE;

static nn_model_handle_t s_model_handle = NULL;

extern const unsigned char *kws_model_ptr;
extern const char *kws_labels[];
extern unsigned int kws_labels_num;

namespace VoiceRelay {
struct Suspended : State {
  Suspended(State *back_state) : back_state_(back_state) {}
  State *clone() { return new Suspended(*this); }
  ~Suspended() {
    if (back_state_) {
      delete back_state_;
    }
  }

  void enterAction(App *app) {
    ESP_LOGI(TAG, "Entering suspended state");
    xEventGroupSetBits(xStatusEventGroup, STATUS_STATE_SUSPENDED_MSK);
    kws_req_word(1);
    app->p_display->print_string_ln("OFF");
    app->p_display->send();
  }
  void exitAction(App *app) {
    xEventGroupClearBits(xStatusEventGroup, STATUS_STATE_SUSPENDED_MSK);
    ESP_LOGI(TAG, "Exiting suspended state");
    app->p_display->clear();
    app->p_display->send();
  }
  void handleEvent(App *app, eEvent ev) {
    switch (ev) {
    case eEvent::TOUCH_CLICK:
    case eEvent::TOUCH_SWIPE_LEFT:
    case eEvent::TOUCH_SWIPE_RIGHT:
      kws_req_cancel();
      app->transition(back_state_);
      back_state_ = nullptr;
      break;
    case eEvent::KWS_WORD: {
      int category;
      char result[32] = {0};
      if (xQueueReceive(xKWSResultQueue, &category, pdMS_TO_TICKS(1000)) ==
          pdPASS) {
        nn_model_get_label(s_model_handle, category, result, sizeof(result));
        if (strcmp(result, "robot") == 0) {
          app->transition(back_state_);
          back_state_ = nullptr;
        }
      } else {
        ESP_LOGW(TAG, "Unable to receive kws word");
      }
    } break;
    default:
      if (uxQueueMessagesWaiting(xKWSRequestQueue) == 0) {
        kws_req_word(1);
      }
      break;
    }
  }

protected:
  State *back_state_;
};

struct Main : State {
  Main() : timer_val_s_(0) {}

  State *clone() { return new Main(*this); }
  void enterAction(App *app) {
    ESP_LOGI(TAG, "Entering main state");
    kws_req_word(1);
    gpio_set_level(LOCK_PIN, 1);
    gpio_set_level(LOCK_PIN_INV, 0);
    xEventGroupSetBits(xStatusEventGroup, STATUS_STATE_UNLOCKED_MSK);
    xTimerChangePeriod(xTimer, SUSPEND_AFTER_TICKS, 0);
    timer_val_s_ = float(SUSPEND_AFTER_TICKS * portTICK_PERIOD_MS) / 1000;
    ESP_LOGD(TAG, "timer_val_s_=%d", timer_val_s_);
  }
  void exitAction(App *app) {
    ESP_LOGI(TAG, "Exiting main state");
    xEventGroupClearBits(xStatusEventGroup, STATUS_STATE_UNLOCKED_MSK);
    gpio_set_level(LOCK_PIN, 0);
    gpio_set_level(LOCK_PIN_INV, 1);
    app->p_display->clear();
    app->p_display->send();
  }
  void handleEvent(App *app, eEvent ev) {
    switch (ev) {
    case eEvent::KWS_WORD: {
      int category;
      char result[32] = {0};
      if (xQueueReceive(xKWSResultQueue, &category, pdMS_TO_TICKS(1000)) ==
          pdPASS) {
        nn_model_get_label(s_model_handle, category, result, sizeof(result));
        if (strcmp(result, "stop") == 0) {
          app->transition(new Suspended(clone()));
        }
      } else {
        ESP_LOGW(TAG, "Unable to receive kws word");
      }
    } break;
    case eEvent::TOUCH_CLICK:
    case eEvent::TOUCH_SWIPE_LEFT:
    case eEvent::TOUCH_SWIPE_RIGHT:
    case eEvent::TIMEOUT:
      kws_req_cancel();
      app->transition(new Suspended(clone()));
      break;
    default:
      if (uxQueueMessagesWaiting(xKWSRequestQueue) == 0) {
        kws_req_word(1);
      }
      break;
    }
  }
  void update(App *app) {
    const TickType_t xRemainingTime =
      xTimerGetExpiryTime(xTimer) - xTaskGetTickCount();
    const size_t rem_time_s = float(xRemainingTime * portTICK_PERIOD_MS) / 1000;
    if (timer_val_s_ != rem_time_s) {
      timer_val_s_ = rem_time_s;
      ESP_LOGD(TAG, "timer_val_s_=%d", timer_val_s_);
      app->p_display->clear();
      app->p_display->print_string_ln("ON:%d", timer_val_s_ + 1);
      app->p_display->send();
    }
  }

private:
  size_t timer_val_s_;
};
} // namespace VoiceRelay

void releaseScenario(App *app) {
  ESP_LOGI(TAG, "Exiting VoiceRelay scenairo");
  kws_event_task_release();
  kws_task_release();
  if (s_model_handle) {
    nn_model_release(s_model_handle);
    s_model_handle = NULL;
  }
}

void initScenario(App *app) {
  int errors = (nn_model_init(&s_model_handle,
                              nn_model_config_t{
                                .model_ptr = kws_model_ptr,
                                .labels = kws_labels,
                                .labels_num = kws_labels_num,
                                .is_quantized = false,
                                .inference_threshold = KWS_INFERENCE_THRESHOLD,
                              }) < 0);
  errors += kws_task_init(kws_task_conf_t{
              .model_handle = s_model_handle,
              .mic_gain = 30,
              .ns_level = 1,
            }) < 0;
  errors += kws_event_task_init() < 0;

  if (errors) {
    ESP_LOGE(TAG, "Unable to init KWS");
    app->transition(nullptr);
  } else {
    ESP_LOGI(TAG, "Entering VoiceRelay scenairo");
    app->transition(new VoiceRelay::Suspended(new VoiceRelay::Main));
  }
}
