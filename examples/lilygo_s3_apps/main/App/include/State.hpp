#pragma once
#include "Event.hpp"

class App;

/*!
 * \brief App state interface.
 */
struct State {
  virtual ~State() = default;
  virtual void enterAction(App *) {}
  virtual void exitAction(App *) {}
  virtual void handleEvent(App *, eEvent) {}
  virtual void update(App *) {}
  virtual State *clone() = 0;
};
