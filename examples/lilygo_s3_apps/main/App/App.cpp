#include "App.hpp"
#include "DisplaySTDOUT.hpp"
#include "Event.hpp"
#include "Lcd_GC9D01N.hpp"
#include "Led_APA102.hpp"
#include "Touch.hpp"

#include "i2s_rx_slot.h"

static constexpr char TAG[] = "App";

App::App() : current_state_(nullptr) {
  gpio_reset_pin(LOCK_PIN);
  gpio_set_direction(LOCK_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(LOCK_PIN, 0);

  gpio_reset_pin(LOCK_PIN_INV);
  gpio_set_direction(LOCK_PIN_INV, GPIO_MODE_OUTPUT);
  gpio_set_level(LOCK_PIN_INV, 1);

  int errors = 0;
  transition_queue_ = xQueueCreate(1, sizeof(State *));
  if (transition_queue_ == NULL) {
    ESP_LOGE(TAG, "Error creating transition queue");
    errors++;
  }
  p_led = new Led_APA102();

#if CONFIG_TARGET_LILYGO_T_CIRCLE
  p_display = new Lcd(Lcd::Rotation::PORTRAIT);
#else
  p_display = new DisplaySTDOUT();
#endif

  p_display->clear();
  p_display->send();

  i2s_rx_slot_init();

  errors += initEventsGenerator() < 0;
  errors += initTouch() < 0;
  errors += initStatusMonitor(p_led) < 0;

  if (errors) {
    ESP_LOGE(TAG, "Init errors=%d", errors);
    xEventGroupSetBits(xStatusEventGroup, STATUS_STATE_BAD_MSK);
    vTaskDelay(portMAX_DELAY);
  }

  initScenario(this);
}

App::~App() {
  releaseScenario(this);

  p_display->clear();
  p_display->send();

  delete p_display;

  p_led->set(0);

  delete p_led;

  State *state;
  while (xQueueReceive(transition_queue_, &state, 0) == pdPASS) {
    delete state;
  }
  if (current_state_) {
    delete current_state_;
  }
  vQueueDelete(transition_queue_);
  releaseEventsGenerator();
  releaseTouch();
  releaseStatusMonitor();
  i2s_rx_slot_release();
}

void App::transition(State *target_state) {
  xQueueSend(transition_queue_, &target_state, portMAX_DELAY);
}

void App::do_transition(State *target_state) {
  if (current_state_ != nullptr) {
    current_state_->exitAction(this);
    delete current_state_;
    current_state_ = nullptr;
  }
  if (target_state != nullptr) {
    target_state->enterAction(this);
  }
  current_state_ = target_state;
  xQueueReset(xEventQueue);
}

void App::run() {
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

  for (;;) {
    State *target_state = nullptr;
    if (xQueueReceive(transition_queue_, &target_state, 0) == pdPASS) {
      do_transition(target_state);
    }
    if (current_state_ == nullptr) {
      return;
    }
    eEvent ev;
    if (xQueueReceive(xEventQueue, &ev, xTicksToWait) == pdPASS) {
      current_state_->handleEvent(this, ev);
    } else {
      current_state_->update(this);
    }
  }
}
