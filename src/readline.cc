#include "readline.h"

#include <unistd.h>
#include <signal.h>
#include <termcap.h>
#include <termios.h>
#include <iostream>
#include <sys/types.h>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>

#include "key-events.h"

namespace readline {

struct termios old_tio;

static void intHandler(int sig) {
  if (sig == SIGINT) {
    std::cout << "^C\n";

    // restore the former settings
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
  }
}


Readline::Readline(size_t hist_size): hist_(hist_size) {
  Log::Instance("log.txt");
}

void Readline::SetCompleteFunc(FuncComplete&& fn) {
  fn_complete_ = std::move(fn);
}

void Readline::SetHighlightFunc(FuncHighlight&& fn) {
  fn_highlight_ = std::move(fn);
}

boost::optional<std::string> Readline::Prompt(const Text& prompt) {
  FuncComplete fn(fn_complete_);
  FuncHighlight fn_highlight(fn_highlight_);
  struct termios new_tio;
  KeyEvents key_events(hist_);

  struct sigaction psa;
  psa.sa_handler = intHandler;
  sigaction(SIGINT, &psa, nullptr);

  // get the terminal settings for stdin
  tcgetattr(STDIN_FILENO,&old_tio);

  // we want to keep the old setting to restore them a the end
  new_tio=old_tio;

  // disable canonical mode (buffered i/o) and local echo
  new_tio.c_lflag &=(~ICANON & ~ECHO);

  // set the new settings immediately
  tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

  boost::optional<std::wstring> line = key_events.Loop(prompt, std::move(fn),
      std::move(fn_highlight));

  // restore the former settings
  tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
  std::cout << "\n";

  if (!line) {
    return boost::optional<std::string>{};
  }

  std::string ret_line = wstr2str(*line);
  boost::trim(ret_line);
  return ret_line;
}

void Readline::AddHistoryString(const std::string cmd) {
  hist_.Push(cmd);
}

const std::list<std::string>& Readline::GetHistoryList() const {
  return hist_.GetList();
}

size_t Readline::CountHistoryItems() const {
  return hist_.Count();
}

}  // namespace readline
