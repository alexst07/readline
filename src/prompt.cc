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
  int char_pos = cursor_.GetPos();

  if (char_pos == 0) {
    return;
  }

  buf_.RemoveChar(char_pos - 1);
  Reprint();
  cursor_.MoveBackward(1);
}

void Prompt::AddChar(char c) {
  int char_pos = cursor_.GetPos();

  if (cursor_.IsInLastPosition(buf_.Length())) {
    cursor_.SetStartLine(cursor_.GetStartLine() - 1);
    std::cout << '\n';
  }

  buf_.AddChar(c, char_pos);
  Reprint();
  cursor_.MoveToPos(char_pos + 1);
}

void Prompt::AdvanceCursor() {
  int char_pos = cursor_.GetPos();

  if (char_pos >= buf_.Length()) {
    return;
  }

  cursor_.MoveToPos(char_pos + 1);
}

void Prompt::BackwardCursor() {
  cursor_.MoveBackward(1);
}

void Prompt::CursorToEnd() {
  int len = buf_.Length();
  cursor_.MoveToPos(len);
}

void Prompt::CursorToBegin() {
  cursor_.MoveToPos(0);
}

void Prompt::Delete() {
  int char_pos = cursor_.GetPos();
  int buf_len = buf_.Length();

  if (char_pos == buf_len) {
    return;
  }

  buf_.RemoveChar(char_pos);
  Reprint();
  cursor_.MoveToPos(char_pos);
}

void Prompt::EraseFromBeginToEnd() {
  int num_lines = NumOfLines();

  for (int i = 0; i < num_lines; i++) {
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + i, 1);
    std::cout << "\033[K";
  }
}

void Prompt::ToNextToken() {
  int char_pos = cursor_.GetPos();
  int pos = buf_.EndTokenPos(char_pos);
  cursor_.MoveToPos(pos);
}

void Prompt::ToBackwardToken() {
  int char_pos = cursor_.GetPos();
  int pos = buf_.StartTokenPos(char_pos);
  cursor_.MoveToPos(pos);
}

void Prompt::RemoveBackwardToken() {
  int char_pos = cursor_.GetPos();
  int pos = buf_.StartTokenPos(char_pos);
  buf_.RemoveSubStr(pos, char_pos);
  Reprint();
  cursor_.MoveToPos(pos);
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
