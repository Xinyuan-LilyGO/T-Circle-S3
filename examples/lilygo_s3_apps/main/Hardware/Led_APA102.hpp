#pragma once

#include "ILed.hpp"

class Led_APA102 : public ILed {
public:
  Led_APA102();
  void set(bool state) override final;
  void set(Colour colour, int led_num = -1, Brightness b = _100) override final;
  void flash(Colour colour, unsigned nums = 1) override final;
};
