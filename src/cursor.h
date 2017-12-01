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

  void MoveTo(int line, int col);

  void MovePositionTo(int line, int col);

  void MoveToPos(int n);

  void MoveToAbsolute(int line, int col);

  void MoveToStartNextLine();

  void MoveForward(int n);

  void MoveBackward(int n);

  inline int GetPosLine() {
    return CalcLine(pos_);
  }

  inline int GetPosCol() {
    return CalcCol(pos_);
  }

  inline int GetPos() {
    return pos_;
  }

  inline int GetStartLine() {
    return start_line_;
  }

  inline int GetStartCol() {
    return start_col_;
  }

  inline int CalcLine(int n) {
    TermSize term_size = Terminal::Size();

    int line = static_cast<int>(std::ceil(static_cast<float>(n+1+start_col_)/
        static_cast<float>(term_size.cols)));
    return line;
  }

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

  int pos_;
  int start_line_;
  int start_col_;
};

Position GetCursorPosition();

}  // namespace readline

#endif  // READLINE_CURSOR_H
