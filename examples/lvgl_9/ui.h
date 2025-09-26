/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2024-11-28 17:07:50
 * @LastEditTime: 2025-09-26 17:28:13
 * @License: GPL 3.0
 */
#pragma once

#include "lvgl.h"
#include "pin_config.h"

typedef struct
{
    const uint32_t win_width = LCD_WIDTH;
    const uint32_t win_height = LCD_HEIGHT;

    lv_obj_t *home;
    lv_obj_t *home_tileview;
    lv_obj_t *home_tile_1;
    lv_obj_t *home_tile_2;

} lv_ui;

void ui_init(lv_ui *ui);
void win_home_init(lv_ui *ui);


extern lv_ui system_ui;