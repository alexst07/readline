#include "cursor.h"

#include <unistd.h>
#include <termcap.h>
#include <termios.h>
#include <string>
#include <iostream>

namespace readline {

Position Cursor::GetPosition() {
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

void Cursor::MoveTo(int line, int col) {
  std::string escape = std::string("\033[") + std::to_string(line) +
      std::string(";") + std::to_string(col) + std::string("f");

  std::cout << escape;
}

void Cursor::MoveTo(const Position& pos) {
  MoveTo(pos.line, pos.col);
}

void Cursor::MoveLinesUp(int n) {
  std::string escape = std::string("\033[") + std::to_string(n) +
      std::string("A");

  std::cout << escape;
}

void Cursor::MoveLinesDown(int n) {
  std::string escape = std::string("\033[") + std::to_string(n) +
      std::string("B");

  std::cout << escape;
}

void Cursor::MoveToCol(int col) {
  Position pos = GetPosition();

  MoveTo(pos.line, col);
}

void Cursor::MoveColsForward(int n) {
  std::string escape = std::string("\033[") + std::to_string(n) +
      std::string("C");

  std::cout << escape;
}

void Cursor::MoveColsBackward(int n) {
  std::string escape = std::string("\033[") + std::to_string(n) +
      std::string("D");

  std::cout << escape;
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

}  // namespace readline
