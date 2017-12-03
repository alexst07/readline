#include "key-events.h"

#include <iostream>
#include <string>
#include <stdio.h>

namespace readline {

std::string KeyEvents::Loop(const std::string& msg, FuncComplete&& fn) {
  Prompt prompt(msg, std::move(fn));
  unsigned char c;

  do {
    c = getchar();

    switch (c) {
      case 8:  // CTRL+backspace
        prompt.RemoveBackwardToken();
        break;

      case 9: // TAB
        prompt.AutoComplete();
        break;

      case 127:  // backspace
        prompt.Backspace();
        break;

      case 27: {
        unsigned char c1 = getchar();
        if (c1 == 91) {
          CommandsKey(prompt);
        } else {  // case where ESQ key was pressed
          c = c1;
        }

        break;
      }

      default:
        prompt.AddChar(c);
     }
  } while (c != '\n');

  return prompt.Str();
}

void KeyEvents::CommandsKey(Prompt& prompt) {
  unsigned char c = getchar();

  switch (c) {
    case 65:  // up arrow key
      prompt.UpArrow();
      break;

    case 66:  // down arrow key
      prompt.DownArrow();
      break;

    case 67:  // right arrow key
      prompt.RightArrow();
      break;

    case 68:  // left arrow key
      prompt.LeftArrow();
      break;

    case 72:  // HOME key
      prompt.CursorToBegin();
      break;

    case 70:  // END key
      prompt.CursorToEnd();
      break;

    case 51: {
      unsigned char c1 = getchar();
      if (c1 == 126) {  // DELETE key
        prompt.Delete();
      }
      break;
    }

    case 49: {
      char c = getchar();
      if (c == 59) {
        CtrlCommandsKey(prompt);
      }
      break;
    }
  }
}

void KeyEvents::CtrlCommandsKey(Prompt& prompt) {
  char c = getchar();

  if (c == 53) {
    char c = getchar();

    switch (c) {
      case 67:  // CTRL+RIGHT_ARROW
        prompt.ToNextToken();
        break;

      case 68:  // CTRL+RIGHT_ARROW
        prompt.ToBackwardToken();
        break;
    }
  }
}

}  // namespace readline
