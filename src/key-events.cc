#include "key-events.h"

#include <iostream>
#include <string>
#include <stdio.h>

namespace readline {

std::string KeyEvents::Loop(const std::string& msg) {
  Prompt prompt(msg);
  unsigned char c;

  do {
    c = getchar();

    switch (c) {
      case 127:  // backspace
        prompt.Backspace();
        break;

      default:
        prompt.AddChar(c);
     }
  } while (c != '\n');

  return prompt.Str();
}

}  // namespace readline
