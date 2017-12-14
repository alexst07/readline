#include "readline.h"

#include <unistd.h>
#include <termcap.h>
#include <termios.h>
#include <iostream>
#include <sys/types.h>

#include "key-events.h"

namespace readline {

void Readline::SetCompleteFunc(FuncComplete&& fn) {
  fn_complete_ = std::move(fn);
}

std::string Readline::Prompt(const std::string& prompt) {
  FuncComplete fn(fn_complete_);
  struct termios old_tio, new_tio;
  KeyEvents key_events(hist_);

  // get the terminal settings for stdin
  tcgetattr(STDIN_FILENO,&old_tio);

  // we want to keep the old setting to restore them a the end
  new_tio=old_tio;

  // disable canonical mode (buffered i/o) and local echo
  new_tio.c_lflag &=(~ICANON & ~ECHO);

  // set the new settings immediately
  tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

  std::string line = key_events.Loop(prompt, std::move(fn));

  // restore the former settings
  tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

  return line;
}

void Readline::AddHistoryString(const std::string cmd) {
  hist_.Push(cmd);
}

}  // namespace readline
