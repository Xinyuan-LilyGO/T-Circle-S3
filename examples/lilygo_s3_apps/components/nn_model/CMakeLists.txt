set(3RDPARTY_DIR "${PROJECT_DIR}/3rdparty")

set(NMSIS_DIR "${3RDPARTY_DIR}/NMSIS/NMSIS/")
set(RISCV_MATH_SRC
    "${NMSIS_DIR}/DSP/Source/FastMathFunctions/riscv_cos_f32.c"
    "${NMSIS_DIR}/DSP/Source/CommonTables/riscv_common_tables.c"
    "${NMSIS_DIR}/DSP/Source/CommonTables/riscv_const_structs.c"
    "${NMSIS_DIR}/DSP/Source/TransformFunctions/riscv_rfft_fast_f32.c"
    "${NMSIS_DIR}/DSP/Source/TransformFunctions/riscv_cfft_f32.c"
    "${NMSIS_DIR}/DSP/Source/TransformFunctions/riscv_cfft_radix8_f32.c"
    "${NMSIS_DIR}/DSP/Source/TransformFunctions/riscv_bitreversal2.c"
    "${NMSIS_DIR}/DSP/Source/TransformFunctions/riscv_rfft_fast_init_f32.c"
    "${NMSIS_DIR}/DSP/Source/TransformFunctions/riscv_cfft_init_f32.c")
set(RISCV_MATH_INC "${NMSIS_DIR}/Core/Include/" "${NMSIS_DIR}/DSP/Include/")

idf_component_register(
  SRCS
  "nn_model.cpp"
  "audio_preprocessor/audio_preprocessor.cpp"
  ${RISCV_MATH_SRC}
  INCLUDE_DIRS
  "./"
  "./audio_preprocessor"
  ${RISCV_MATH_INC}
  REQUIRES
  "esp_timer"
  "esp-tflite-micro")

target_compile_options(
  ${COMPONENT_LIB}
  PRIVATE -Wno-error=unused-const-variable -Wno-error=delete-non-virtual-dtor
          -Wno-error=implicit-function-declaration -fpermissive)
