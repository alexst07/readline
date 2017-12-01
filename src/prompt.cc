#include "prompt.h"

#include <cmath>
#include <iostream>
#include <string>
#include "cursor.h"

namespace readline {

Prompt::Prompt(const std::string& str_prompt)
    : str_prompt_(str_prompt)
    , cursor_(GetCursorPosition().line, str_prompt_.length()) {
  std::cout << str_prompt_ << std::flush;
  cursor_.MoveToPos(0);
}

void Prompt::Backspace() {
  if (buf_.Length() == 0) {
    return;
  }

  int char_pos = cursor_.GetPos();
  buf_.RemoveChar(char_pos - 1);
  Reprint();
  cursor_.MoveBackward(1);
}

void Prompt::AddChar(char c) {
  int char_pos = cursor_.GetPos();
  buf_.AddChar(c, char_pos);
  Reprint();
  cursor_.MoveToPos(char_pos + 1);
}

void Prompt::EraseFromBeginToEnd() {
  int num_lines = NumOfLines();

  for (int i = 0; i < num_lines; i++) {
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + i, 1);
    std::cout << "\033[K";
  }
}

int Prompt::NumOfLines() {
  return cursor_.CalcLine(buf_.Length());
}

void Prompt::Reprint() {
  EraseFromBeginToEnd();

  cursor_.MoveToAbsolute(cursor_.GetStartLine(), 1);
  std::string content = str_prompt_ + buf_.Str();
  std::cout << content << std::flush;
}

}  // namespace readline
