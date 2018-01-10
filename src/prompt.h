#ifndef READLINE_PROMPT_H
#define READLINE_PROMPT_H

#include "buffer-string.h"
#include "cursor.h"
#include "complete.h"
#include "history.h"

namespace readline {

using FuncHighlight = std::function<Text(const std::string&)>;

class Prompt {
 public:
  Prompt(const Text& str_prompt, History& hist, FuncComplete&& fn,
    FuncHighlight&& fn_highlight);

  bool Enter();

  void Backspace();

  void AddChar(wchar_t c);

  void Search();

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

  void Tab();

  void EnterCompleteMode();

  void AutoComplete();

  void CleanScreen();

  int NumOfLines();

  void AddLines(int n);

  void Esq();

  void ShowTip(std::string tip);

  void ShowTipForPath(std::string tip);

  void HideTip();

  void AcceptTip();

  Cursor& GetCursorRef() {
    return cursor_;
  }

  inline const std::wstring& WStr() const {
    return buf_.WStr();
  }

  inline const std::string Str() const {
    return buf_.Str();
  }

  inline bool IsInCompleteMode() {
    return complete_.Showing();
  }

  void SetStartLine(int n);

 private:
  void EraseFromBeginToEnd();

  void Reprint();

  void SearchPrint();

  int PosCursonOnBuf();

  void SearchExec(const std::wstring& search_cmd);

  inline bool AlwaysShowComplete() {
    bool show_always = false;
    if (cursor_.GetPos() != buf_.Length()) {
      show_always = true;
    }

    return show_always;
  }

  Text str_prompt_;
  BufferString buf_;
  Cursor cursor_;
  Complete complete_;
  bool tip_mode_;
  std::string tip_string_;
  std::string original_tip_string_;
  History& hist_;
  std::wstring current_cmd_;
  std::wstring search_cmd_;
  FuncHighlight fn_highlight_;
};

}  // namespace readline

#endif  // READLINE_PROMPT_H
