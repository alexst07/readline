#ifndef READLINE_READLINE_H
#define READLINE_READLINE_H

#include <string>
#include <functional>

#include "complete.h"
#include "history.h"
#include "prompt.h"

namespace readline {

class Readline {
 public:
  Readline(): hist_(500) {}

  void SetCompleteFunc(FuncComplete&& fn);

  void SetHighlightFunc(FuncHighlight&& fn);

  std::wstring Prompt(const Text& prompt);

  void AddHistoryString(const std::string cmd);

 private:
  FuncComplete fn_complete_;
  FuncHighlight fn_highlight_;
  History hist_;
};

}  // namespace readline

#endif  // READLINE_READLINE_H
