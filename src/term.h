#ifndef READLINE_TERM_H
#define READLINE_TERM_H

#include <memory>
#include <vector>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <termcap.h>
#include <termios.h>
#include <iostream>
#include <sys/types.h>

namespace readline {

void term_change_mode();

void term_restore_mode();
}

#endif  // READLINE_TERM_H
