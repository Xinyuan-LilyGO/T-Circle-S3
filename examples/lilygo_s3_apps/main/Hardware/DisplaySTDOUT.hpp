#pragma once

#include "IDisplay.hpp"
#include "stdio.h"

class DisplaySTDOUT : public IDisplay {
public:
  void setFont(Font f) const override final {}
  void setRotation(Rotation rot) const override final {}
  void print_string_ln(const char *fmt, ...) override final {
    va_list argp;
    va_start(argp, fmt);

    vprintf(fmt, argp);
    printf("\n");

    va_end(argp);
  }
  void print_string(unsigned x, unsigned y, const char *fmt,
                    ...) override final {
    va_list argp;
    va_start(argp, fmt);

    vprintf(fmt, argp);
    printf("\n");

    va_end(argp);
  }
  void draw(unsigned x, unsigned y, unsigned w, unsigned h,
            const uint8_t *bitmap) override final {}
  void send() override final {}
  void clear() override final {}
};
