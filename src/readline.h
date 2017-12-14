#ifndef READLINE_READLINE_H
#define READLINE_READLINE_H

#include <string>
#include <functional>

#include "complete.h"
#include "history.h"

namespace readline {

class Readline {
 public:
  Readline(): hist_(500) {}

  void SetCompleteFunc(FuncComplete&& fn);

  std::string Prompt(const std::string& prompt);

  void AddHistoryString(const std::string cmd);

 private:
  FuncComplete fn_complete_;
  History hist_;
};

}  // namespace readline

#endif  // READLINE_READLINE_H
