#include "complete.h"

#include <string>
#include <functional>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "cursor.h"

namespace readline {

Complete::Complete(const std::string& line, int line_pos, int start_line,
    FuncType&& fn, Cursor& cursor, Prompt& prompt)
    : line_(line)
    , line_pos_(line_pos)
    , start_line_(start_line)
    , item_sel_(0)
    , fn_complete_(std::move(fn))
    , complete_token_(false)
    , cursor_(cursor)
    , prompt_(prompt) {}

void Complete::Print() {
  if (fn_complete_) {
    std::vector<std::string> args;
    split(args, line_, boost::algorithm::is_space());
    bool new_arg = CheckNewArg();
    CompleteList list_result(fn_complete_(args, new_arg));
  }
}

void Complete::PrintList(const std::vector<std::string>& list) {
  int len = 0;

  // get the max string len item
  for (const auto& item: list) {
    if (item.length() > len) {
      len = item.length();
    }
  }

  TermSize term_size = Terminal::Size();
  int ncols = term_size.cols / len;

  if (ncols > 1) {
    ncols = (term_size.cols - 3*ncols)/len;
    
  }

}

void Complete::PrintItemsList(const std::vector<std::string>& list, int nc,
    int len) {
  int lines = 1;

  for (int i = 0; i < list.size(); i++) {
    int pos_col = (i%nc)* len + (i > 0? 2: 0);
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + prompt_.NumOfLines()+lines,
        pos_col + 1);
    std::cout << list[0];

    if (i%nc == 0) {
      ++lines;
    }
  }

  cursor_.MoveToPos(cursor_.GetPos());
}

bool Complete::CheckNewArg() {
  if (line_pos_ == 0) {
    return true;
  }

  if (line_[line_pos_ - 1] == ' ') {
    return true;
  }

  return false;
}

}  // namespace readline
