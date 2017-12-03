#include "prompt.h"

#include <cmath>
#include <iostream>
#include <string>
#include "cursor.h"
#include "complete.h"

namespace readline {

Prompt::Prompt(const std::string& str_prompt, FuncComplete&& fn)
    : str_prompt_(str_prompt)
    , cursor_(GetCursorPosition().line, str_prompt_.length())
    , complete_(cursor_.GetStartLine(), std::move(fn), *this)
    , tip_mode_(false) {
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

  if (complete_.Showing()) {
    complete_.Show(buf_.Str(), cursor_.GetPos());
  }
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

  if (complete_.Showing()) {
    complete_.Show(buf_.Str(), cursor_.GetPos());

    // if a space char is pressed, and this space was not escaped by \ char
    // we have to hide the completation menu
    if (c == ' ') {
      if (!buf_.IsEscapeSpace(char_pos)) {
        complete_.Hide();
      }
    }
  }
}

void Prompt::RightArrow() {
  if (IsInCompleteMode()) {
    complete_.SelNextItem();
  } else {
    AdvanceCursor();
  }
}

void Prompt::LeftArrow() {
  if (IsInCompleteMode()) {
    complete_.SelBackItem();
  } else {
    BackwardCursor();
  }
}

void Prompt::UpArrow() {
  if (IsInCompleteMode()) {
    complete_.SelUpItem();
  }
}

void Prompt::DownArrow() {
  if (IsInCompleteMode()) {
    complete_.SelDownItem();
  }
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
  // erase all lines that was used to show the prompt
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

void Prompt::AutoComplete() {
  std::string sel_content = complete_.UseSelContent();
  complete_.Show(buf_.Str(), cursor_.GetPos());
}

void Prompt::Esq() {
  if (IsInCompleteMode()) {
    complete_.Hide();
  }
}

int Prompt::NumOfLines() {
  return cursor_.CalcLine(buf_.Length());
}

void Prompt::SetStartLine(int n) {
  cursor_.SetStartLine(n);
  Reprint();
}

void Prompt::AddLines(int n) {
  // Add lines with \n, and put the start line on the correct place
  for (int i = 0; i < n; i++) {
    std::cout << '\n';
  }

  cursor_.SetStartLine(cursor_.GetStartLine() - n);
  Reprint();
}

void Prompt::ShowTip(const std::string& tip) {
  int char_pos = cursor_.GetPos();

  if (!buf_.IsLastToken(char_pos)) {
    return;
  }

  // if the cursor is not in the end of the token, we have to verify the
  // intersection between last token, and the tip, to avoid repeat part of
  // the string
  if (buf_[char_pos-1] != ' ') {

  }


  tip_mode_ = true;
  int len = buf_.Length();
  cursor_.MoveOnlyCursorToPos(len);
  std::cout << "\e[38;5;239m" << tip << "\033[0m";

  cursor_.MoveToPos(char_pos);
}

void Prompt::Reprint() {
  EraseFromBeginToEnd();

  cursor_.MoveToAbsolute(cursor_.GetStartLine(), 1);
  std::string content = str_prompt_ + buf_.Str();
  std::cout << content << std::flush;
}

}  // namespace readline
