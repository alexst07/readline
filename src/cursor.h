#ifndef READLINE_CURSOR_H
#define READLINE_CURSOR_H

namespace readline {

struct Position {
  int line;
  int col;
};

struct TermSize {
  int lines;
  int cols;
};

class Cursor {
 public:
  static Position GetPosition();

  static void MoveTo(int line, int col);

  static void MoveTo(const Position& pos);

  static void MoveToCol(int col);

  static void MoveLinesUp(int n);

  static void MoveLinesDown(int n);

  static void MoveColsForward(int n);

  static void MoveColsBackward(int n);
};

class Terminal {
 public:
  static TermSize Size();
};

}  // namespace readline

#endif  // READLINE_CURSOR_H
