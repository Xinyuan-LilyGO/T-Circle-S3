#include "Lcd_GC9D01N.hpp"

#include "Arduino_GFX_Library.h"
#include "driver/gpio.h"

static constexpr char TAG[] = "Lcd";

// H0075Y002-V0
#define LCD_MOSI 17
#define LCD_SCLK 15
#define LCD_DC   16
#define LCD_RST  -1
#define LCD_CS   13
#define LCD_BL   18

Arduino_DataBus *s_bus;
Arduino_GFX *s_gfx;

Lcd::Lcd(Rotation rot) : IDisplay() {
  s_bus = new Arduino_ESP32SPIDMA(LCD_DC /* DC */, LCD_CS /* CS */,
                                  LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */,
                                  -1 /* MISO */);

  s_gfx = new Arduino_GC9D01N(s_bus, LCD_RST /* RST */, 0 /* rotation */,
                              false /* IPS */, DISPLAY_WIDTH /* width */,
                              DISPLAY_HEIGHT /* height */, 0 /* col offset 1 */,
                              0 /* row offset 1 */, 0 /* col_offset2 */,
                              0 /* row_offset2 */);

  gpio_reset_pin(gpio_num_t(LCD_BL));
  gpio_set_direction(gpio_num_t(LCD_BL), GPIO_MODE_OUTPUT);
  gpio_set_level(gpio_num_t(LCD_BL), 0);

  s_gfx->begin();
  s_gfx->fillScreen(BLACK);
}

Lcd::~Lcd() {
  delete s_bus;
  delete s_gfx;
}

void Lcd::setRotation(Rotation rot) const {
  switch (rot) {
  default:
  case Rotation::PORTRAIT:
    s_gfx->setRotation(0);
    break;
  case Rotation::UPSIDE_DOWN:
    s_gfx->setRotation(1);
    break;
  }
}

void Lcd::setFont(Font f) const {
  switch (f) {
  default:
  case Font::COURR08:
    s_gfx->setFont(u8g_font_courR08);
    break;
  case Font::COURR10:
    s_gfx->setFont(u8g_font_courR10);
    break;
  case Font::COURB10:
    s_gfx->setFont(u8g_font_courB10);
    break;
  case Font::COURR14:
    s_gfx->setFont(u8g_font_courR14);
    break;
  case Font::COURB24:
    s_gfx->setFont(u8g_font_courB24);
    break;
  case Font::F6X10TR:
    s_gfx->setFont(u8g2_font_6x10_tr);
    break;
  case Font::F8X13B:
    s_gfx->setFont(u8g_font_8x13B);
    break;
  case Font::F9X15:
    s_gfx->setFont(u8g_font_9x15);
    break;
  case Font::F9X18:
    s_gfx->setFont(u8g_font_9x18);
    break;
  }
}

void Lcd::draw_string(unsigned x, unsigned y, const char *fmt, va_list argp) {
  const unsigned buf_size = 32;
  char buf[buf_size];
  memset(buf, 0, buf_size);
  vsnprintf(buf, buf_size, fmt, argp);

  int16_t x1, y1;
  uint16_t w, h;
  s_gfx->setTextColor(WHITE);
  s_gfx->setTextSize(5);
  s_gfx->getTextBounds(buf, x, y, &x1, &y1, &w, &h);
  x1 = (DISPLAY_WIDTH - w) / 2;
  y1 = (DISPLAY_HEIGHT - h) / 2;
  s_gfx->setCursor(x1, y1);
  s_gfx->print(buf);
}

void Lcd::print_string_ln(const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  draw_string(0, 0, fmt, argp);
  va_end(argp);
}

void Lcd::print_string(unsigned x, unsigned y, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  draw_string(x, y, fmt, argp);
  va_end(argp);
}

void Lcd::draw(unsigned x, unsigned y, unsigned w, unsigned h,
               const uint8_t *bitmap) {
  s_gfx->drawXBitmap((DISPLAY_WIDTH - w) / 2, (DISPLAY_WIDTH - w) / 2, bitmap,
                     w, h, WHITE);
}

void Lcd::send() {}

void Lcd::clear() { s_gfx->fillScreen(BLACK); }
