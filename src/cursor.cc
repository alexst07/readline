#include "cursor.h"

#include <unistd.h>
#include <termcap.h>
#include <termios.h>
#include <string>
#include <iostream>

namespace readline {

Cursor::Cursor(int start_line, int start_col)
    : pos_(0)
    , start_line_(start_line)
    , start_col_(start_col) {}

void Cursor::MoveToStartNextLine() {
  TermSize term_size = Terminal::Size();

  int line = CalcLine(pos_) - 1 + start_line_;
  pos_ = line * term_size.cols - start_col_;

  MoveToScape(line + start_line_ + 1, 1);
}

void Cursor::MovePositionTo(int line, int col) {
  TermSize term_size = Terminal::Size();
  pos_ = line*term_size.cols - start_col_ - (term_size.cols - col);

  int rline = CalcLine(pos_) - 1 + start_line_;
  int rcol = CalcCol(pos_);

  MoveToScape(rline + start_line_, rcol);
}

void Cursor::MoveTo(int line, int col) {
  TermSize term_size = Terminal::Size();
  int pos = line*term_size.cols - start_col_ - (term_size.cols - col);

  int rline = CalcLine(pos) - 1 + start_line_;
  int rcol = CalcCol(pos);

  MoveToScape(rline + start_line_, rcol);
}

void Cursor::MoveToAbsolute(int line, int col) {
  std::string escape = std::string("\033[") +
    std::to_string(line) + std::string(";") +
    std::to_string(col) + std::string("f");

  std::cout << escape;
}

void Cursor::MoveToPos(int n) {
  int line = CalcLine(n) - 1 + start_line_;
  int col = CalcCol(n);
  pos_ = n;

  MoveToScape(line, col);
}

void Cursor::MoveForward(int n) {
  pos_ += n;
  MoveToPos(pos_);
}

void Cursor::MoveBackward(int n) {
  pos_ -= n;
  MoveToPos(pos_);
}

TermSize Terminal::Size() {
  static char termbuf[2048];
  char *termtype = getenv("TERM");

  tgetent(termbuf, termtype);

  int lines = tgetnum("li");
  int columns = tgetnum("co");

  TermSize term_size;
  term_size.lines = lines;
  term_size.cols = columns;

  return term_size;
}

Position GetCursorPosition() {
  std::cout << "\033[6n" << std::flush;

  char c = 0;
  std::string str_line, str_col;

  // marks to help scanner the string
  bool brackets = false;
  bool comma = false;

  // scanner a string with format: <bytes>[<line>;<col>R
  while (c != 'R') {
   std::cin.get(c);

   // get the line position
   if (brackets && !comma && (c >= '0' && c <= '9')) {
     str_line += c;
   }

   // get the col position
   if (comma && (c >= '0' && c <= '9')) {
     str_col += c;
   }

   if (c == ';') {
     comma = true;
   }

   if (c == '[') {
     brackets = true;
   }
  }

  Position pos;
  pos.line = std::stoi(str_line,nullptr,10);
  pos.col = std::stoi(str_col,nullptr,10);

  return pos;
}

}  // namespace readline
