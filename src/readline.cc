#include "readline.h"

#include <unistd.h>
#include <termcap.h>
#include <termios.h>
#include <iostream>
#include <sys/types.h>

#include "key-events.h"

namespace readline {

std::string Readline::Prompt(const std::string& prompt) {
  struct termios old_tio, new_tio;
  KeyEvents key_events;

  // get the terminal settings for stdin
  tcgetattr(STDIN_FILENO,&old_tio);

  // we want to keep the old setting to restore them a the end
  new_tio=old_tio;

  // disable canonical mode (buffered i/o) and local echo
  new_tio.c_lflag &=(~ICANON & ~ECHO);

  // set the new settings immediately
  tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

  std::string line = key_events.Loop(prompt);

  // restore the former settings
  tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

  return line;
}

}  // namespace readline
