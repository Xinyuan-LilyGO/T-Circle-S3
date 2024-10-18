set(3RDPARTY_DIR "${PROJECT_DIR}/3rdparty")

set(U8G2_DIR "${3RDPARTY_DIR}/u8g2")
set(U8G2_SRC
    "${U8G2_DIR}/csrc/u8x8_setup.c"
    "${U8G2_DIR}/csrc/u8x8_8x8.c"
    "${U8G2_DIR}/csrc/u8x8_display.c"
    "${U8G2_DIR}/csrc/u8x8_cad.c"
    "${U8G2_DIR}/csrc/u8x8_byte.c"
    "${U8G2_DIR}/csrc/u8x8_gpio.c"
    "${U8G2_DIR}/csrc/u8x8_d_ssd1306_128x64_noname.c"
    "${U8G2_DIR}/csrc/u8g2_setup.c"
    "${U8G2_DIR}/csrc/u8g2_cleardisplay.c"
    "${U8G2_DIR}/csrc/u8g2_buffer.c"
    "${U8G2_DIR}/csrc/u8g2_fonts.c"
    "${U8G2_DIR}/csrc/u8g2_font.c"
    "${U8G2_DIR}/csrc/u8g2_hvline.c"
    "${U8G2_DIR}/csrc/u8g2_ll_hvline.c"
    "${U8G2_DIR}/csrc/u8g2_intersection.c"
    "${U8G2_DIR}/csrc/u8g2_d_setup.c"
    "${U8G2_DIR}/csrc/u8g2_d_memory.c"
    "${U8G2_DIR}/cppsrc/U8x8lib.cpp")
set(U8G2_INC "${U8G2_DIR}/cppsrc" "${U8G2_DIR}/csrc")

if(${CONFIG_APP_VOICE_RELAY})
  set(KWS_SRC "kws/VoiceRelay.cpp" "kws/kws_event_task.cpp" "kws/kws_model.cpp"
              "kws/kws_task.cpp")
  set(KWS_INC "kws")

  add_compile_definitions(KWS_INFERENCE_THRESHOLD=0.9)

  set(APP_SCENARIO_SRC ${KWS_SRC})
  set(APP_SCENARIO_INC ${KWS_INC})
elseif(${CONFIG_APP_SOUND_EVENTS_DETECTION})
  set(SED_SRC
      "sed/SED.cpp" "sed/sed_task.cpp" "sed/sed_model_baby_cry.cpp"
      "sed/sed_model_glass_breaking.cpp" "sed/sed_model_bark.cpp"
      "sed/sed_model_coughing.cpp")
  set(SED_INC "sed")

  add_compile_definitions(SED_INFERENCE_THRESHOLD=0.9)

  set(APP_SCENARIO_SRC ${SED_SRC})
  set(APP_SCENARIO_INC ${SED_INC})
endif()

set(GFX_DIR "${3RDPARTY_DIR}/Arduino_GFX-1.3.7")
set(GFX_SRC
    "${GFX_DIR}/src/databus/Arduino_ESP32SPIDMA.cpp"
    "${GFX_DIR}/src/Arduino_DataBus.cpp"
    "${GFX_DIR}/src/Arduino_GFX.cpp"
    "${GFX_DIR}/src/Arduino_TFT.cpp"
    "${GFX_DIR}/src/Arduino_G.cpp"
    "${GFX_DIR}/src/display/Arduino_GC9D01N.cpp"
    )
set(GFX_INC "${GFX_DIR}/src" "${GFX_DIR}/src/display" "${GFX_DIR}/src/databus")

set(FAST_LED_DIR "${3RDPARTY_DIR}/FastLED-3.6.0")
set(FAST_LED_SRC "${FAST_LED_DIR}/src/FastLED.cpp")
set(FAST_LED_INC "${FAST_LED_DIR}/src")

set(BUS_DIR "${3RDPARTY_DIR}/Arduino_DriveBus-1.1.12")
set(TOUCH_SRC
    "${BUS_DIR}/src/hardware/Arduino_HWIIC.cpp"
    "${BUS_DIR}/src/touch_chip/Arduino_CST816x.cpp"
    "${BUS_DIR}/src/Arduino_DriveBus.cpp"
    "${BUS_DIR}/src/Arduino_DriveBus_Library.cpp"
    "${BUS_DIR}/src/Arduino_IIC.cpp")
set(TOUCH_INC "${BUS_DIR}/src")

idf_component_register(
  SRCS
  "main.cpp"
  "./App/App.cpp"
  "./App/Event.cpp"
  "./App/Status.cpp"
  "./App/Touch.cpp"
  "./Hardware/Led_APA102.cpp"
  "./Hardware/Lcd_GC9D01N.cpp"
  ${U8G2_SRC}
  ${GFX_SRC}
  ${FAST_LED_SRC}
  ${TOUCH_SRC}
  ${APP_SCENARIO_SRC}
  INCLUDE_DIRS
  "./"
  "./App/include"
  "./Hardware"
  ${U8G2_INC}
  ${GFX_INC}
  ${FAST_LED_INC}
  ${TOUCH_INC}
  ${APP_SCENARIO_INC}
  REQUIRES
  "mic_reader"
  "nn_model"
  "esp_timer"
  "esp_lcd"
  "driver")

target_compile_options(
  ${COMPONENT_LIB}
  PRIVATE -Wno-error=unused-const-variable -Wno-error=delete-non-virtual-dtor
          -Wno-error=implicit-function-declaration -fpermissive)

add_compile_definitions(SUSPEND_TIMEOUT_S=10)
