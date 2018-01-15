#ifndef READLINE_KEY_EVENTS_H
#define READLINE_KEY_EVENTS_H

#include <string>
#include <boost/optional.hpp>
#include "cursor.h"
#include "prompt.h"
#include "history.h"

namespace readline {

class KeyEvents {
 public:
  KeyEvents(History& hist): hist_(hist) {}

  boost::optional<std::wstring> Loop(const Text& msg, FuncComplete&& fn,
      FuncHighlight&& fn_highlight);

  void CommandsKey(Prompt& prompt);

  void CtrlCommandsKey(Prompt& prompt);

 private:
  void SpecialChars(Prompt& prompt, wchar_t c1);

  History& hist_;
};

}  // namespace readline

#endif  // READLINE_KEY_EVENTS_H
