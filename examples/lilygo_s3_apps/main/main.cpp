/*
 * @Description: None
 * @Author: None
 * @Date: 2024-10-11 09:29:06
 * @LastEditTime: 2024-10-18 17:25:51
 * @License: GPL 3.0
 */
#include "App.hpp"
#include "git_version.h"

extern "C" void app_main(void) {
  printf("VERSION: %s\n", VERSION_STRING);
  App app;
  app.run();
}
