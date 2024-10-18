#pragma once

#include <stdint.h>

/*!
 * \brief Interface to leds.
 */
class ILed {
public:
  enum Colour : unsigned { Red = 0, Green, Blue, Yellow, White, Cyan, Black };
  enum Brightness : unsigned {
    _100 = 0,
    _50 = 1,
    _25 = 2,
  };

  /*!
   * \brief Constructor.
   * \param pin GPIO pin.
   */
  ILed(int pin) : pin_(pin) {}
  virtual ~ILed() = default;
  /*!
   * \brief Set state.
   * \param state Led state.
   */
  virtual void set(bool state) = 0;
  /*!
   * \brief Set state.
   * \param colour Led colour.
   * \param led_num Led pixel number.
   * \param b Brightness.
   */
  virtual void set(Colour colour, int led_num = -1, Brightness b = _50) = 0;
  /*!
   * \brief Flash with leds.
   * \param colour Led colour.
   * \param nums Number of times.
   */
  virtual void flash(Colour colour, unsigned nums) = 0;

protected:
  /*! \brief GPIO pin. */
  int pin_;
};
