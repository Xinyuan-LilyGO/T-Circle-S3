/*
 * @Description: lvgl 9 example
 * @Author: LILYGO_L
 * @Date: 2025-01-08 09:08:52
 * @LastEditTime: 2025-09-26 18:04:44
 * @License: GPL 3.0
 */
#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "ui.h"
#include "Arduino_DriveBus_Library.h"

// #define LV_DISPLAY_ROTATION LV_DISPLAY_ROTATION_0
#define LV_DISPLAY_ROTATION LV_DISPLAY_ROTATION_90

Arduino_DataBus *bus = new Arduino_ESP32SPIDMA(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_GFX *gfx = new Arduino_GC9D01N(
    bus, LCD_RST /* RST */, 0 /* rotation */, false /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

lv_ui system_ui;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> CST816D(new Arduino_CST816x(IIC_Bus, CST816D_DEVICE_ADDRESS,
                                                         TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void)
{
    CST816D->IIC_Interrupt_Flag = true;
}

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    int32_t src_w = lv_area_get_width(area);
    int32_t src_h = lv_area_get_height(area);

    uint8_t *rotated_buf;

    lv_display_rotation_t rotation = lv_display_get_rotation(disp);
    lv_area_t rotated_area;
    if (rotation != LV_DISPLAY_ROTATION_0)
    {
        lv_color_format_t cf = lv_display_get_color_format(disp);
        /*Calculate the position of the rotated area*/
        rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);
        /*Calculate the source stride (bytes in a line) from the width of the area*/
        uint32_t src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
        /*Calculate the stride of the destination (rotated) area too*/
        uint32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&rotated_area), cf);
        /*Have a buffer to store the rotated area and perform the rotation*/

        rotated_buf = (uint8_t *)heap_caps_malloc(LCD_WIDTH * LCD_HEIGHT * (LV_COLOR_DEPTH / 8), MALLOC_CAP_SPIRAM);
        assert(rotated_buf);
        lv_draw_sw_rotate(px_map, rotated_buf, src_w, src_h, src_stride, dest_stride, rotation, cf);
        /*Use the rotated area and rotated buffer from now on*/
        area = &rotated_area;
        px_map = rotated_buf;
    }

    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, src_w, src_h);

    /*Call it to tell LVGL you are ready*/
    lv_display_flush_ready(disp);

    if (rotation != LV_DISPLAY_ROTATION_0)
    {
        heap_caps_free(rotated_buf);
    }
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    int32_t Touch_x, Touch_y;
    uint8_t touchpad = 0;

    // 读取手指数量
    touchpad = (uint8_t)CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);

    if (touchpad > 0)
    {
        Touch_x = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
        Touch_y = CST816D->IIC_Read_Device_Value(CST816D->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

        data->state = LV_INDEV_STATE_PR;
        data->point.x = Touch_x;
        data->point.y = Touch_y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}

void lvgl_init(void)
{
    lv_init();

    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

    lv_display_t *display;
    /*Else create a display yourself*/
    display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);

    size_t draw_buffer_sz = LCD_WIDTH * LCD_HEIGHT * sizeof(lv_color_t);
    void *buf1 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_SPIRAM);
    assert(buf1);
    // void *buf2 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_SPIRAM);
    // assert(buf2);
    // initialize LVGL draw buffers
    lv_display_set_buffers(display, buf1, NULL, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_FULL);

    lv_display_set_flush_cb(display, my_disp_flush);

    /*Initialize the (dummy) input device driver*/
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, my_touchpad_read);

    lv_display_set_rotation(display, LV_DISPLAY_ROTATION);
}

void setup()
{
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.begin(115200);
    Serial.println(LVGL_Arduino);

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 0);

    while (CST816D->begin() == false)
    {
        Serial.println("CST816D initialization fail");
        delay(2000);
    }
    Serial.println("CST816D initialization successfully");

    // 中断模式为检测到触摸时，发出低脉冲
    CST816D->IIC_Write_Device_State(CST816D->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
                                    CST816D->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);

    gfx->begin(40000000);
    gfx->fillScreen(BLACK);

    lvgl_init();
    ui_init(&system_ui);
}

void loop()
{
    lv_timer_handler();
    delay(5); /* let this time pass */
}
