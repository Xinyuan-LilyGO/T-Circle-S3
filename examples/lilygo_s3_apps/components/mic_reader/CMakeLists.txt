idf_component_register(
  SRCS
  "i2s_rx_slot.cpp"
  INCLUDE_DIRS
  "./"
  PRIV_REQUIRES
  "driver")

target_compile_options(
  ${COMPONENT_LIB}
  PRIVATE -Wno-error=unused-const-variable -Wno-error=delete-non-virtual-dtor
          -Wno-error=implicit-function-declaration -fpermissive)
