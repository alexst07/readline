#ifndef READLINE_PROMPT_H
#define READLINE_PROMPT_H

#include "buffer-string.h"

namespace readline {

class Prompt {
 public:
  Prompt(const std::string& str_prompt = "");

  void Backspace();

  void AddChar(char c);

  void AdvanceCursor();

  void BackwardCursor();

  inline const std::string& Str() const {
    return buf_.Str();
  }

 private:
  void EraseFromBeginToEnd();

  void Reprint();

  int NumOfLines();

  int PosCursonOnBuf();

  int start_col_;
  int start_line_;
  std::string str_prompt_;
  BufferString buf_;
};

}  // namespace readline

#endif  // READLINE_PROMPT_H
