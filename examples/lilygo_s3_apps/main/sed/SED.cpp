#include "App.hpp"
#include "Status.hpp"
#include "git_version.h"
#include "sed_task.h"

static constexpr char TAG[] = "SED";
extern const unsigned char *sed_baby_cry_model_ptr;
extern const char *sed_baby_cry_labels[];
extern unsigned int sed_baby_cry_labels_num;

extern const unsigned char *sed_glass_breaking_model_ptr;
extern const char *sed_glass_breaking_labels[];
extern unsigned int sed_glass_breaking_labels_num;

extern const unsigned char *sed_bark_model_ptr;
extern const char *sed_bark_labels[];
extern unsigned int sed_bark_labels_num;

extern const unsigned char *sed_coughing_model_ptr;
extern const char *sed_coughing_labels[];
extern unsigned int sed_coughing_labels_num;

static nn_model_handle_t s_model_handle = NULL;

struct {
  const char *name;
  const unsigned char *model_ptr;
  const char **labels;
  unsigned int labels_num;
  int mic_gain;
} static const model_desc {
#if CONFIG_SOUND_EVENTS_BABY_CRY
  .name = "baby_cry", .model_ptr = sed_baby_cry_model_ptr,
  .labels = sed_baby_cry_labels, .labels_num = sed_baby_cry_labels_num,
  .mic_gain = 25,
#elif CONFIG_SOUND_EVENTS_GLASS_BREAKING
  .name = "glass_breaking", .model_ptr = sed_glass_breaking_model_ptr,
  .labels = sed_glass_breaking_labels,
  .labels_num = sed_glass_breaking_labels_num, .mic_gain = 6,
#elif CONFIG_SOUND_EVENTS_BARK
  .name = "bark", .model_ptr = sed_bark_model_ptr, .labels = sed_bark_labels,
  .labels_num = sed_bark_labels_num, .mic_gain = 20,
#elif CONFIG_SOUND_EVENTS_COUGHING
  .name = "coughing", .model_ptr = sed_coughing_model_ptr,
  .labels = sed_coughing_labels, .labels_num = sed_coughing_labels_num,
  .mic_gain = 25,
#else
#error "set sound events type"
#endif
};

namespace SED {
struct Main : State {
  State *clone() override final { return new Main(*this); }
  void handleEvent(App *app, eEvent ev) override final {
    switch (ev) {
    default:
      break;
    }
  }
  void enterAction(App *app) override final {
    app->p_display->clear();
    app->p_display->print_string_ln("OFF");
    app->p_display->send();
  }
  void exitAction(App *app) override final {
    app->p_display->clear();
    app->p_display->send();
  }
  void update(App *app) override final {
    static char label[32];
    static int category;
    if (xQueuePeek(xSEDResultQueue, &category, 0) == pdPASS) {
      xEventGroupSetBits(xStatusEventGroup, STATUS_STATE_UNLOCKED_MSK);
      gpio_set_level(LOCK_PIN, 1);
      gpio_set_level(LOCK_PIN_INV, 0);
      nn_model_get_label(s_model_handle, category, label, sizeof(label));
      ESP_LOGI(TAG, "Detected: %s", label);
      app->p_display->clear();
      app->p_display->print_string_ln("ON");
      app->p_display->send();
      vTaskDelay(pdMS_TO_TICKS(1000));
      xQueueReceive(xSEDResultQueue, &category, 0);
      gpio_set_level(LOCK_PIN, 0);
      gpio_set_level(LOCK_PIN_INV, 1);
      xEventGroupClearBits(xStatusEventGroup, STATUS_STATE_UNLOCKED_MSK);
      app->p_display->clear();
      app->p_display->print_string_ln("OFF");
      app->p_display->send();
    }
  }
};
} // namespace SED

void releaseScenario(App *app) {
  ESP_LOGI(TAG, "Exiting SED scenairo");
  sed_task_release();
  if (s_model_handle) {
    nn_model_release(s_model_handle);
    s_model_handle = NULL;
  }
}

void initScenario(App *app) {
  ESP_LOGI(TAG, "Entering SED (%s) scenairo", model_desc.name);
  int errors = nn_model_init(&s_model_handle,
                             nn_model_config_t{
                               .model_ptr = model_desc.model_ptr,
                               .labels = model_desc.labels,
                               .labels_num = model_desc.labels_num,
                               .is_quantized = true,
                               .inference_threshold = SED_INFERENCE_THRESHOLD,
                             }) < 0;
  errors += sed_task_init(sed_task_conf_t{
              .model_handle = s_model_handle,
              .mic_gain = model_desc.mic_gain,
            }) < 0;
  if (errors) {
    ESP_LOGE(TAG, "SED init errors=%d", errors);
    app->transition(nullptr);
  } else {
    app->transition(new SED::Main);
  }
}
