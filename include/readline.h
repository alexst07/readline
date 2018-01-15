#ifndef READLINE_READLINE_H
#define READLINE_READLINE_H

#include <string>
#include <functional>
#include <boost/optional.hpp>

#include "complete.h"
#include "history.h"
#include "prompt.h"
#include "utils.h"

namespace readline {

class Readline {
 public:
  Readline(size_t hist_size = 500);

  void SetCompleteFunc(FuncComplete&& fn);

  void SetHighlightFunc(FuncHighlight&& fn);

  boost::optional<std::string> Prompt(const Text& prompt);

  void AddHistoryString(const std::string cmd);

  const std::list<std::string>& GetHistoryList() const;

  size_t CountHistoryItems() const;

 private:
  FuncComplete fn_complete_;
  FuncHighlight fn_highlight_;
  History hist_;
};

}  // namespace readline

#endif  // READLINE_READLINE_H
