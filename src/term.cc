#include "term.h"

namespace readline {

static struct termios old_tio;

void term_change_mode() {
  struct termios new_tio;

  // get the terminal settings for stdin
  tcgetattr(STDIN_FILENO,&old_tio);

  // we want to keep the old setting to restore them a the end
  new_tio=old_tio;

  // disable canonical mode (buffered i/o) and local echo
  new_tio.c_lflag &=(~ICANON & ~ECHO);

  // set the new settings immediately
  tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
}

void term_restore_mode() {
  // restore the former settings
  tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
}

}
