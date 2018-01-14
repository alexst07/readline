#include "key-events.h"

#include <iostream>
#include <string>
#include <stdio.h>

namespace readline {

std::wstring KeyEvents::Loop(const Text& msg, FuncComplete&& fn,
    FuncHighlight&& fn_highlight) {
  Prompt prompt(msg, hist_, std::move(fn), std::move(fn_highlight));
  wchar_t c;

  do {
  START:
    c = getchar();

    if (c == '\n') {
      bool used = prompt.Enter();

      if (used) {
        goto START;
      } else {
        break;
      }
    }

    switch (c) {
      case 8:  // CTRL+backspace
        prompt.RemoveBackwardToken();
        break;

      case 12:  // CTRL+L
        prompt.CleanScreen();
        break;

      case 18:  // CTRL+backspace
        prompt.Search();
        break;

      case 9: // TAB
        prompt.Tab();
        break;

      case 127:  // backspace
        prompt.Backspace();
        break;

      case 195: // special chars
        SpecialChars(prompt, c);
        goto START;
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

  return prompt.WStr();
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

void KeyEvents::SpecialChars(Prompt& prompt, wchar_t c1) {
  wchar_t c = getchar();
  wchar_t ch = c1 | c;

  prompt.AddChar(ch);
}

}  // namespace readline
