#include "prompt.h"

#include <cmath>
#include <iostream>
#include <string>
#include "cursor.h"

namespace readline {

Prompt::Prompt(const std::string& str_prompt)
    : str_prompt_(str_prompt) {
  std::cout << str_prompt_ << std::flush;
  Position pos = Cursor::GetPosition();
  start_col_ = pos.col;
  start_line_ = pos.line;
}

void Prompt::Backspace() {
  if (buf_.Length() == 0) {
    return;
  }

  Position pos = Cursor::GetPosition();
  bool backward_cursor = false;
  bool put_cursor_on_first = false;

  if (pos.col == 1) {
    backward_cursor = true;
  }

  if (pos.col == 2) {
    put_cursor_on_first = true;
  }

  int char_pos = PosCursonOnBuf();

  buf_.RemoveChar(char_pos - 1);
  Reprint();

  if (backward_cursor) {
    // after the Reprint the col pos change, so we can't call function
    // backward_cursor because this function check if col pos == 1
    TermSize term_size = Terminal::Size();
    Cursor::MoveTo(pos.line - 1, term_size.cols);
  }

  if (put_cursor_on_first) {
    Position pos = Cursor::GetPosition();
    Cursor::MoveTo(pos.line + 1, 1);
  }
}

void Prompt::AddChar(char c) {
  Position pos = Cursor::GetPosition();
  TermSize term_size = Terminal::Size();
  bool advance_cursor = false;

  if (pos.col == term_size.cols) {
    advance_cursor = true;
  }

  int char_pos = PosCursonOnBuf();
  buf_.AddChar(c, char_pos);
  Reprint();

  if (advance_cursor) {
    AdvanceCursor();
  }
}

void Prompt::EraseFromBeginToEnd() {
  int num_lines = NumOfLines();

  if (num_lines > 1) {
    // if there are more than one line, we have to iterate and clear all others
    // lines
    for (int i = start_line_ + 1; i < start_line_ + num_lines; i++) {
      Cursor::MoveTo(i, 0);
      std::cout << "\033[K" << std::flush;
    }
  }

  // in any case we erase the first line
  Cursor::MoveTo(start_line_, start_col_);
  std::cout << "\033[K" << std::flush;
}

int Prompt::NumOfLines() {
  TermSize term_size = Terminal::Size();
  int str_len = buf_.Length();
  int prompt_size = str_prompt_.length();
  int max_str_len_on_first_line = term_size.cols - prompt_size - 1;

  if (str_len > max_str_len_on_first_line) {
    float div = static_cast<float>(str_len - max_str_len_on_first_line)/
        static_cast<float>(term_size.cols);
    return static_cast<int>(std::ceil(div)) + 1;
  }

  return 1;
}

int Prompt::PosCursonOnBuf() {
  int num_lines = NumOfLines();
  Position pos = Cursor::GetPosition();
  int char_pos = 0;

  if (num_lines > 1) {
    // if there are more than one line, we have to check in which line is the
    // cursor, if it is on first line, so we have to calculate the position
    // with start_col_, but if is on other line, we only subtract one
    if (pos.line == start_line_) {
      char_pos = pos.col - start_col_;
    } else {
      // we have to sum all chars on the others lines
      int num_cols = Terminal::Size().cols;
      int num_chars_before = num_cols - start_col_ + 1 + num_cols*(pos.line - start_line_ - 1);
      char_pos = num_chars_before + pos.col;
    }
  } else {
    // if there is only one line, we calculate the position where the cursor
    // is on the string
    char_pos = pos.col - start_col_;
  }

  return char_pos;
}

void Prompt::AdvanceCursor() {
  Position pos = Cursor::GetPosition();
  TermSize term_size = Terminal::Size();

  if (pos.col == term_size.cols) {
    if (pos.line == term_size.lines) {
      std::cout << "\n";
    }
    
    Cursor::MoveTo(pos.line + 1, 1);
  } else {
    Cursor::MoveColsForward(1);
  }
}

void Prompt::BackwardCursor() {
  Position pos = Cursor::GetPosition();
  TermSize term_size = Terminal::Size();

  if (pos.col == 1) {
    Cursor::MoveTo(pos.line - 1, term_size.cols);
  } else {
    Cursor::MoveColsBackward(1);
  }
}

void Prompt::Reprint() {
  EraseFromBeginToEnd();

  if (buf_.Length() > 0) {
    std::cout << buf_ << std::flush;
  }
}

}  // namespace readline
