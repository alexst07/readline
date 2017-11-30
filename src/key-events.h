#ifndef READLINE_KEY_EVENTS_H
#define READLINE_KEY_EVENTS_H

#include "cursor.h"
#include "prompt.h"

namespace readline {

class KeyEvents {
 public:
  KeyEvents() = default;

  std::string Loop(const std::string& msg);

 private:

};

}  // namespace readline

#endif  // READLINE_KEY_EVENTS_H
