#ifndef READLINE_CURSOR_H
#define READLINE_CURSOR_H

#include <iostream>
#include <cmath>

namespace readline {

struct Position {
  int line;
  int col;
};

struct TermSize {
  int lines;
  int cols;
};

class Terminal {
 public:
  static TermSize Size();
};

class Cursor {
 public:
  Cursor(int start_line, int start_col);

  // move to relative line and relative col, and NOT update the pos_
  void MoveTo(int line, int col);

  // move to relative line and relative col, and update the pos_
  void MovePositionTo(int line, int col);

  // move the pos_ to n, and calculate the new line and the new col where
  // the terminal cursor must be
  void MoveToPos(int n);

  // move the pos_ to n, and calculate the new line and the new col where
  // the terminal cursor must be, but doesn't update pos_
  void MoveOnlyCursorToPos(int n);

  // move the terminator cursor to absolute value of line and col, it doesn't
  // update the pos_
  void MoveToAbsolute(int line, int col);

  // move the terminal cursor to the start of next line, it updates the pos_
  void MoveToStartNextLine();

  // increment pos_ value, and move the terminal cursor the next position, if
  // it is on the end of the line, it moves to the next line
  void MoveForward(int n);

  // decrement pos_ value, and move the terminal cursor the backward position,
  // if it is on the start of the line, it moves to the backward line
  void MoveBackward(int n);

  // verify if the value n is in the last position on terminal
  // the value of n, is calculate like pos_
  bool IsInLastPosition(int n);

  // get the relative line where the terminal cursor is
  inline int GetPosLine() {
    return CalcLine(pos_);
  }

  // get the relative col where the terminal cursor is
  inline int GetPosCol() {
    return CalcCol(pos_);
  }

  // get the pos_
  inline int GetPos() {
    return pos_;
  }

  inline int GetStartLine() {
    return start_line_;
  }

  // on the last line of terminal we have to update start_line_ for one line
  // backward
  inline void SetStartLine(int n) {
    start_line_ = n;
  }

  inline int GetStartCol() {
    return start_col_;
  }

  // calculates the line where the value of n is
  inline int CalcLine(int n) {
    TermSize term_size = Terminal::Size();

    // with ceil, the line start at 1
    int line = static_cast<int>(std::ceil(static_cast<float>(n+1+start_col_)/
        static_cast<float>(term_size.cols)));
    return line;
  }

  inline int CalcAbsoluteLine(int n) {
    return CalcLine(n) + start_line_ - 1;
  }

  // calculates the col where the value of n is
  inline int CalcCol(int n) {
    TermSize term_size = Terminal::Size();

    int col = (n + start_col_)%term_size.cols + 1;
    return col;
  }

 private:
  inline void MoveToScape(int line, int col) {
    std::string escape = std::string("\033[") +
      std::to_string(line) + std::string(";") +
      std::to_string(col) + std::string("f");

    std::cout << escape << std::flush;
  }

  // it is the posistion that represent the string buffer, it is a pointer
  // where the terminal cursor is on this string
  int pos_;

  // line of terminal where prompt starts
  int start_line_;

  // col of terminal where the prompt start, after the message
  int start_col_;
};

Position GetCursorPosition();

}  // namespace readline

#endif  // READLINE_CURSOR_H
