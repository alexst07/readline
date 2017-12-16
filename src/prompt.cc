#include "prompt.h"

#include <cmath>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include "cursor.h"
#include "complete.h"
#include "utils.h"
#include "log.h"

namespace readline {

const char* SEARCH_PROMPT = "Search ";

Prompt::Prompt(const std::string& str_prompt, History& hist, FuncComplete&& fn)
    : str_prompt_(str_prompt)
    , cursor_(GetCursorPosition().line, str_prompt_.length())
    , complete_(std::move(fn), *this)
    , tip_mode_(false)
    , hist_(hist) {
  std::cout << str_prompt_ << std::flush;
  cursor_.MoveToPos(0);
}

void Prompt::Enter() {
  if (IsInCompleteMode()) {
    EnterCompleteMode();
  } else if (tip_mode_) {
    AcceptTip();
  }

  if (hist_.IsInSearchMode()) {
    hist_.ExitSearchMode();
    Reprint();
  }
}

void Prompt::EnterCompleteMode() {
  LOG << "[EnterCompleteMode]\n";
  // get the argument
  int char_pos = cursor_.GetPos();
  int start_word = buf_.StartArgPos(char_pos);
  int end_word = buf_.EndArgPos(char_pos);

  // use the content of menu that was selected
  std::string str_comp = complete_.UseSelContent();

  // if it is tip for path, we have to handle different from a normal token
  // we have to calculate intersection string with last part of path
  if (complete_.IsPathComplete()) {
    // get the argument only until where is the cursor, becuase we want
    // eliminate the rest of the path
    std::string trim_token = buf_.GetSlice(start_word, char_pos);
    std::string arg_path_init;
    std::string last_part;
    size_t last_bar_pos = trim_token.find_last_of("/");

    if (last_bar_pos != std::string::npos) {
      arg_path_init = trim_token.substr(0, last_bar_pos + 1);
    }

    // insert '/' on the end of directory
    str_comp = DirectoryFormat(arg_path_init + str_comp);
  }

  int current_length = buf_.Length();

  // replace the trim token by the selected option
  buf_.ReplaceStringInterval(str_comp, start_word, end_word);

  complete_.Hide();
  end_word = buf_.EndArgPos(char_pos);

  HideTip();
  Reprint();
  cursor_.MoveToPos(end_word);

  // insert whitespace if it is in the end of string and is not directory
  if (char_pos == current_length && str_comp[str_comp.length() - 1] != '/') {
    buf_.AddString(" ", buf_.Length());
    cursor_.MoveToPos(buf_.Length());
  }
}

void Prompt::Backspace() {
  LOG << "[EnterCompleteMode]\n";

  if (hist_.IsInSearchMode()) {
    SearchExec(search_cmd_.substr(0, search_cmd_.length()-1));
    return;
  }

  int char_pos = cursor_.GetPos();

  if (char_pos == 0) {
    return;
  }

  buf_.RemoveChar(char_pos - 1);
  Reprint();
  cursor_.MoveBackward(1);

  if (complete_.Showing()) {
    std::vector<std::string> args = SplitArgs(buf_.Str(), cursor_.GetPos());
    bool show_always = AlwaysShowComplete();
    complete_.Show(args, show_always);
  }

  // reset tip, to avoid trash as tip
  std::vector<std::string> args;
  args.push_back("");
  complete_.CompleteTip(args);

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::AddChar(char c) {
  LOG << "[AddChar]\n";

  // if it is in search mode, only add a char on the end of search string
  if (hist_.IsInSearchMode()) {
    search_cmd_ += c;
    SearchExec(search_cmd_);
    return;
  }

  int char_pos = cursor_.GetPos();

  // add new line, if it is on the end of the line
  if (cursor_.IsInLastPosition(buf_.Length())) {
    cursor_.SetStartLine(cursor_.GetStartLine() - 1);
    std::cout << '\n';
  }

  buf_.AddChar(c, char_pos);
  Reprint();
  cursor_.MoveToPos(char_pos + 1);

  if (tip_mode_) {
    LOG << "Show tip from AddChar\n";
    ShowTip(original_tip_string_);
  } else {
    std::vector<std::string> args = SplitArgs(buf_.Str(), cursor_.GetPos());
    LOG << "CompleteTip from AddChar\n";
    complete_.CompleteTip(args);
  }


  if (complete_.Showing()) {
    std::vector<std::string> args = SplitArgs(buf_.Str(), cursor_.GetPos());
    bool show_always = AlwaysShowComplete();
    LOG << "Show from AddChar\n";
    complete_.Show(args, show_always);

    // if a space char is pressed, and this space was not escaped by \ char
    // we have to hide the completation menu
    if (c == ' ') {
      if (!buf_.IsEscapeSpace(char_pos)) {
        complete_.Hide();
      }
    }
  }

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::Search() {
  if (hist_.IsInSearchMode()) {
    SearchExec(search_cmd_);
  } else {
    SearchExec(buf_.Str());
  }
}

void Prompt::SearchExec(const std::string& search_cmd) {
  if (!hist_.IsInSearchMode() ||  search_cmd_ != search_cmd) {
    hist_.SearchQuery(search_cmd);
    search_cmd_ = search_cmd;
  }

  if (hist_.HasSearchResult()) {
    current_cmd_ = search_cmd;
    buf_ = hist_.GetSearchResult();
  }

  Reprint();
}

void Prompt::RightArrow() {
  if (IsInCompleteMode()) {
    complete_.SelNextItem();
  } else if (tip_mode_) {
    AcceptTip();
  } else {
    AdvanceCursor();
  }
}

void Prompt::LeftArrow() {
  if (tip_mode_) {
    HideTip();
  }

  if (IsInCompleteMode()) {
    complete_.SelBackItem();
  } else {
    BackwardCursor();
  }
}

void Prompt::UpArrow() {
  if (tip_mode_) {
    HideTip();
  }

  if (IsInCompleteMode()) {
    complete_.SelUpItem();
  } else {
    LOG << "UpArrow [History.GetNext]\n";
    // select next item from history
    if (hist_.HasNext()) {
      LOG << "UpArrow [History.HasNext]\n";

      if (!hist_.IsInHistMode()) {
        current_cmd_ = buf_.Str();
      }

      buf_ = hist_.GetNext() + " ";
      Reprint();
      cursor_.MoveToPos(buf_.Length());
    }
  }
}

void Prompt::DownArrow() {
  if (tip_mode_) {
    HideTip();
  }

  if (IsInCompleteMode()) {
    complete_.SelDownItem();
  } else {
    LOG << "DownArrow [History.GetPreviews]\n";
    // select next item from history
    if (hist_.HasPreviews()) {
      LOG << "UpArrow [History.HasPreviews]\n";
      buf_ = hist_.GetPreviews() + " ";
      Reprint();
      cursor_.MoveToPos(buf_.Length());
    } else if (hist_.IsInHistMode()) {
      buf_ = current_cmd_;
      Reprint();
      cursor_.MoveToPos(buf_.Length());
    }
  }
}

void Prompt::AdvanceCursor() {
  int char_pos = cursor_.GetPos();

  if (char_pos >= buf_.Length()) {
    return;
  }

  cursor_.MoveToPos(char_pos + 1);

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::BackwardCursor() {
  cursor_.MoveBackward(1);

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::CursorToEnd() {
  int len = buf_.Length();
  cursor_.MoveToPos(len);

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::CursorToBegin() {
  cursor_.MoveToPos(0);

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::Delete() {
  int char_pos = cursor_.GetPos();
  int buf_len = buf_.Length();

  if (char_pos == buf_len) {
    return;
  }

  buf_.RemoveChar(char_pos);
  Reprint();
  cursor_.MoveToPos(char_pos);

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::EraseFromBeginToEnd() {
  // erase all lines that was used to show the prompt
  int num_lines = NumOfLines();

  for (int i = 0; i < num_lines; i++) {
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + i, 1);
    std::cout << "\033[K";
  }
}

void Prompt::ToNextToken() {
  int char_pos = cursor_.GetPos();
  int pos = buf_.EndTokenPos(char_pos);
  cursor_.MoveToPos(pos);
}

void Prompt::ToBackwardToken() {
  int char_pos = cursor_.GetPos();
  int pos = buf_.StartTokenPos(char_pos);
  cursor_.MoveToPos(pos);
}

void Prompt::RemoveBackwardToken() {
  int char_pos = cursor_.GetPos();
  int pos = buf_.StartTokenPos(char_pos);
  buf_.RemoveSubStr(pos, char_pos);
  Reprint();

  if (complete_.Showing()) {
    std::vector<std::string> args = SplitArgs(buf_.Str(), cursor_.GetPos());
    bool show_always = AlwaysShowComplete();
    complete_.Show(args, show_always);
  }

  cursor_.MoveToPos(pos);

  if (hist_.IsInHistMode()) {
    hist_.ExitHistMode();
  }
}

void Prompt::Tab() {
  // if the complete menu is begin showed
  if (IsInCompleteMode()) {
    // check if any item was selected
    if (complete_.IsAnyItemSelected()) {
      // if the list of menu has only one element
      if (complete_.ListSize() == 1) {
        // select the option and complete on prompt
        EnterCompleteMode();
      } else {
        // if there are more than one element, select next element
        complete_.SelNextItem();
      }
    } else {
      // if no element was selected, select the first element
      complete_.SelectFirstItem();
    }
  } else if (tip_mode_) {
    AcceptTip();
  } else {
    AutoComplete();
  }
}

void Prompt::AutoComplete() {
  std::string sel_content = complete_.UseSelContent();
  std::vector<std::string> args = SplitArgs(buf_.Str(), cursor_.GetPos());

  bool show_always = AlwaysShowComplete();
  complete_.Show(args, show_always);
}

void Prompt::Esq() {
  if (IsInCompleteMode()) {
    complete_.Hide();
  }
}

int Prompt::NumOfLines() {
  return cursor_.CalcLine(buf_.Length());
}

void Prompt::SetStartLine(int n) {
  cursor_.SetStartLine(n);
  Reprint();
}

void Prompt::AddLines(int n) {
  // Add lines with \n, and put the start line on the correct place
  for (int i = 0; i < n; i++) {
    std::cout << '\n';
  }

  cursor_.SetStartLine(cursor_.GetStartLine() - n);
  Reprint();
}

void Prompt::ShowTip(std::string tip) {
  LOG << "[ShowTip]\n";
  LOG << "tip: " << tip << "\n";
  int char_pos = cursor_.GetPos();

  if (cursor_.GetPos() != buf_.Length()) {
    // assign false to tip_mode_ to avoid complete with tab
    tip_mode_ = false;
    return;
  }

  original_tip_string_ = tip;
  std::string trim_token = buf_.GetTrimToken(char_pos);

  // if it is tip for path, we have to handle different from a normal token
  // we have to calculate intersection string with last part of path
  if (complete_.IsPathComplete()) {
    std::tie(std::ignore, trim_token) = ParserPath(trim_token,
        /*supress_point*/ true);
  }

  // if the cursor is not in the end of the token, we have to verify the
  // intersection between last token, and the tip, to avoid repeat part of
  // the string
  int find_pos = tip.find(trim_token);

  if (find_pos == std::string::npos) {
    // assign false to tip_mode_ to avoid complete with tab
    tip_mode_ = false;
    return;
  }

  tip_string_ = tip.substr(find_pos + trim_token.length());

  tip_mode_ = true;
  int len = buf_.Length();

  Reprint();
  cursor_.MoveOnlyCursorToPos(len);
  std::cout << "\e[38;5;239m" << tip_string_ << "\033[0m";

  cursor_.MoveToPos(char_pos);
}

void Prompt::AcceptTip() {
  bool is_path = false;
  // tip is only showed on last token, so we have only to add the tip string
  // to the last token
  if (complete_.IsPathComplete()) {
    // insert '/' on the end of directory
    std::vector<std::string> args = SplitArgs(buf_.Str(), cursor_.GetPos());
    std::string str_dir = args.back() + tip_string_;
    if (IsDirectory(str_dir)) {
      tip_string_ += "/";
      is_path = true;
    }
  }

  buf_.AddString(is_path?tip_string_:tip_string_+ " ", buf_.Length());

  tip_string_ = "";
  HideTip();

  if (complete_.Showing()) {
    complete_.Hide();
  }

  cursor_.MoveToPos(buf_.Length());
}

void Prompt::HideTip() {
  int char_pos = cursor_.GetPos();
  Reprint();
  cursor_.MoveToPos(char_pos);
  tip_string_ = "";
  tip_mode_ = false;
}

void Prompt::Reprint() {
  if (hist_.IsInSearchMode()) {
    cursor_.SetStartCol(std::string(SEARCH_PROMPT).length());
    SearchPrint();
    return;
  } else {
    cursor_.SetStartCol(str_prompt_.length());
  }

  EraseFromBeginToEnd();

  cursor_.MoveToAbsolute(cursor_.GetStartLine(), 1);
  std::string content = str_prompt_ + buf_.Str();
  std::cout << content << std::flush;
}

void Prompt::SearchPrint() {
  EraseFromBeginToEnd();

  cursor_.MoveToAbsolute(cursor_.GetStartLine(), 1);
  std::string content = std::string(SEARCH_PROMPT) + "'" + search_cmd_ +
      "': " + buf_.Str();
  std::cout << content << std::flush;
}

}  // namespace readline
