#ifndef READLINE_READLINE_H
#define READLINE_READLINE_H

#include <string>
#include <functional>

#include "complete.h"

namespace readline {

class Readline {
 public:
  Readline() = default;

  void SetCompleteFunc(FuncComplete&& fn);

  std::string Prompt(const std::string& prompt);

 private:
  FuncComplete fn_complete_;
};

}  // namespace readline

#endif  // READLINE_READLINE_H
