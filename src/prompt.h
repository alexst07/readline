#ifndef READLINE_PROMPT_H
#define READLINE_PROMPT_H

#include "buffer-string.h"
#include "cursor.h"
#include "complete.h"

namespace readline {

class Prompt {
 public:
  Prompt(const std::string& str_prompt, FuncComplete&& fn);

  void Backspace();

  void AddChar(char c);

  void RightArrow();

  void LeftArrow();

  void UpArrow();

  void DownArrow();

  void AdvanceCursor();

  void BackwardCursor();

  void CursorToEnd();

  void CursorToBegin();

  void Delete();

  void ToNextToken();

  void ToBackwardToken();

  void RemoveBackwardToken();

  void AutoComplete();

  int NumOfLines();

  void AddLines(int n);

  void Esq();

  void ShowTip(std::string tip);

  Cursor& GetCursorRef() {
    return cursor_;
  }

  inline const std::string& Str() const {
    return buf_.Str();
  }

  inline bool IsInCompleteMode() {
    return complete_.Showing();
  }

  void SetStartLine(int n);

 private:
  void EraseFromBeginToEnd();

  void Reprint();

  int PosCursonOnBuf();

  std::string str_prompt_;
  BufferString buf_;
  Cursor cursor_;
  Complete complete_;
  bool tip_mode_;
  std::string tip_string_;
};

}  // namespace readline

#endif  // READLINE_PROMPT_H
