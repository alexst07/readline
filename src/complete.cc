#include "complete.h"

#include <string>
#include <functional>
#include <vector>
#include <boost/filesystem.hpp>
#include "cursor.h"
#include "prompt.h"
#include "scope-exit.h"
#include "utils.h"

namespace readline {

Complete::Complete(int start_line, FuncComplete&& fn, Prompt& prompt)
    : start_line_(start_line)
    , item_sel_(-1)
    , fn_complete_(std::move(fn))
    , complete_token_(false)
    , cursor_(prompt.GetCursorRef())
    , prompt_(prompt)
    , lines_show_(0)
    , show_(false)
    , num_cols_(0)
    , total_items_(0)
    , show_always_(false)
    , is_path_(false)
    , has_more_(false)
    , full_screen_mode_(false) {}

void Complete::Show(const std::vector<std::string>& args, bool show_always) {
  int pos = cursor_.GetPos();
  if (show_) {
    Hide();
  }

  show_ = true;
  show_always_ = show_always;
  lines_show_ = Print(args);
  cursor_.MoveToPos(pos);
}

void Complete::Show() {
  int pos = cursor_.GetPos();
  if (show_) {
    CleanLines();
  }

  lines_show_ = PrintList(items_);
  cursor_.MoveToPos(pos);
}

void Complete::Hide() {
  CleanLines();
  show_ = false;
  item_sel_ = -1;
  lines_show_ = 0;
  is_path_ = false;
  show_always_ = false;
  total_items_ = 0;
  has_more_ = false;
}

void Complete::CleanLines() {
  for (int i = 0; i <= lines_show_; i++) {
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + i +1, 1);
    std::cout << "\033[K";
  }
}

int Complete::Print(const std::vector<std::string>& args) {
  std::string last_arg = args.back();

  if (fn_complete_) {
    CompleteList list_result(fn_complete_(args));

    if (list_result.GetType() == CompleteList::Type::kList) {
      is_path_ = false;
      items_ = boost::get<CompleteResultList>(list_result.GetItems())
          .GetItems();

      if (last_arg.empty()) {
        return PrintList(items_);
      } else {
        // uses only item that match
        items_ = MatchArg(last_arg, items_);
        return PrintList(items_);
      }
    }
  } else {
    is_path_ = true;
    std::string path;
    std::string last_part;
    std::tie(path, last_part) = ParserPath(last_arg);

    if (!IsDirectory(path)) {
      is_path_ = false;
      show_ = false;
      return 0;
    }

    items_ = ListDir(path, ListDirType::FILES_DIR);

    if (last_part.empty()) {
      return PrintList(items_);
    } else {
      // uses only item that match
      items_ = MatchArg(last_part, items_);
      return PrintList(items_);
    }
  }
}

void Complete::CompleteTip(const std::vector<std::string>& args) {
  std::string last_arg = args.back();
  std::vector<std::string> items;

  if (fn_complete_) {
    CompleteList list_result(fn_complete_(args));

    if (list_result.GetType() == CompleteList::Type::kList) {
      is_path_ = false;
      items = boost::get<CompleteResultList>(list_result.GetItems())
          .GetItems();

      if (!last_arg.empty()) {
        // uses only item that match
        items = MatchArg(last_arg, items);
      }
    }
  } else {
    is_path_ = true;
    std::string path;
    std::string last_part;
    std::tie(path, last_part) = ParserPath(last_arg);

    if (!IsDirectory(path)) {
      is_path_ = false;
      show_ = false;
      return;
    }

    items = ListDir(path, ListDirType::FILES_DIR);

    if (!last_part.empty()) {
      // uses only item that match
      items = MatchArg(last_part, items);
    }
  }

  if (items.size() == 1) {
    sel_content_ = items[0];
    prompt_.ShowTip(items[0]);
    show_ = false;
  }
}

int Complete::PrintList(const std::vector<std::string>& list) {
  if (list.empty()) {
    show_ = false;
    return 0;
  }

  // don't show the list if there is only one element, and show_always_ is false
  // on this case, we only show the tip for the user
  if (list.size() == 1 && !show_always_) {
    sel_content_ = list[0];
    prompt_.ShowTip(list[0]);
    show_ = false;
    return 0;
  }

  size_t len = 0;

  // get the max string len item
  for (const auto& item: list) {
    if (item.length() > len) {
      len = item.length();
    }
  }

  TermSize term_size = Terminal::Size();

  // we add by 3, because we want 3 white space between each item
  int ncols = term_size.cols / (len + 3);

  if (ncols > 1) {
    return PrintItemsList(list, ncols, len);
  } else {
    int lines = 0;

    for (size_t i = 0; i < list.size(); i++) {;
      cursor_.MoveToAbsolute(
          cursor_.GetStartLine() + prompt_.NumOfLines()+lines, 1);
      // clean the line
      std::cout << "\033[K";

      // print item on line
      std::cout << list[i];
      ++lines;
    }

    return lines;
  }
}

int Complete::PrintItemsList(const std::vector<std::string>& list, int nc,
    int len) {
  int lines = 0;
  num_cols_ = nc;
  total_items_ = list.size();

  // calculates the number of needed lines
  int num_lines = static_cast<int>(std::ceil(
      static_cast<float>(list.size()) / static_cast<float>(nc)));

  // calculates where must be start line
  TermSize term_size = Terminal::Size();
  int pos_line = prompt_.GetCursorRef().CalcAbsoluteLine(
      prompt_.Str().length());

  // calculate the number of lines, and how many lines we will need to add
  // '\n' char
  num_lines = num_lines > CalcMaxLines()? CalcMaxLines() + 1: num_lines;

  if (pos_line + num_lines > term_size.lines) {
    int num_add_lines = num_lines - (term_size.lines - pos_line);
    prompt_.AddLines(num_add_lines);
  }

  for (size_t i = 0; i < list.size(); i++) {
    if (i%nc == 0 && i > 0) {
      ++lines;
      cursor_.MoveToAbsolute(
          cursor_.GetStartLine() + prompt_.NumOfLines()+lines, 1);
      // clean the line
      std::cout << "\033[K";

      if (lines >= CalcMaxLines()) {
        PrintMoreOpt(item_sel_ == kMore);
        return lines;
      }
    }

    // show the items side by side, I multiply i by 3 to keep two spaces
    // between each item
    int mod = i%nc;
    int pos_col = mod* len + (mod > 0? mod*3: 0);
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + prompt_.NumOfLines()+lines,
        pos_col + 1);

    if (i == item_sel_) {
      sel_content_ = list[i];
      std::cout << "\e[48;5;7m" << list[i] << "\033[0m";
      prompt_.ShowTip(list[i]);
    } else {
      std::cout << list[i];
    }
  }

  has_more_ = false;
  return lines;
}

void Complete::SelectFirstItem() {
  auto cleanup = MakeScopeExit([&]() {
    Show();
  });
  IgnoreUnused(cleanup);

  item_sel_ = 0;
}

void Complete::SelNextItem() {
  auto cleanup = MakeScopeExit([&]() {
    Show();
  });
  IgnoreUnused(cleanup);

  if (item_sel_ < 0) {
    item_sel_ = 0;
  }

  int total;
  if (has_more_) {
    total = (lines_show_ - 1)*num_cols_;
  } else {
    total = total_items_;
  }

  int sel = item_sel_ + 1;

  if (sel >= total) {
    if (has_more_) {
      item_sel_ = kMore;
      return;
    } else {
      item_sel_ = 0;
      return;
    }
  }

  ++item_sel_;
}

void Complete::SelBackItem() {
  auto cleanup = MakeScopeExit([&]() {
    Show();
  });
  IgnoreUnused(cleanup);

  if (item_sel_ < 0) {
    item_sel_ = 0;
  }

  int sel = item_sel_ - 1;

  if (sel < 0) {
    item_sel_ = total_items_ - 1;
    return;
  }

  --item_sel_;
}

void Complete::SelDownItem() {
  auto show = MakeScopeExit([&]() {
    Show();
  });
  IgnoreUnused(show);

  if (item_sel_ < 0) {
    item_sel_ = 0;
    return;
  }

  int total;
  if (has_more_) {
    total = (lines_show_ - 1)*num_cols_;
  } else {
    total = total_items_;
  }

  int sel = item_sel_ + num_cols_;

  if (item_sel_ == total - 1) {
    if (has_more_) {
      item_sel_ = kMore;
    } else {
      // if all items was showed, we have to check in which collunm we are
      // because if we are not in last collunm, we can't go back to start
      // in fact we have to advance to next collunm
      if (item_sel_ % num_cols_ == num_cols_ - 1) {
        item_sel_ = 0;
      } else {
        item_sel_ = item_sel_%num_cols_ + 1;
      }
    }

    return;
  }

  if (sel >= total) {
    if (item_sel_%num_cols_ == 0 && has_more_) {
      item_sel_ = kMore;
    } else if (item_sel_%num_cols_ == num_cols_ - 1){
      item_sel_ = 0;
    } else {
      item_sel_ = item_sel_%num_cols_ + 1;
    }

    return;
  }

  item_sel_ += num_cols_;
}

void Complete::SelUpItem() {
  auto cleanup = MakeScopeExit([&]() {
    Show();
  });
  IgnoreUnused(cleanup);

  if (item_sel_ < 0) {
    item_sel_ = 0;
    return;
  }

  int sel = item_sel_ - num_cols_;

  if (item_sel_ == 0) {
    item_sel_ = total_items_ - 1;
    return;
  }

  if (sel < 0) {
    item_sel_ = (num_cols_)*(has_more_?
        lines_show_ - 2: lines_show_ - 1) + item_sel_%num_cols_ - 1;
    return;
  }

  item_sel_ -= num_cols_;
}

std::string Complete::UseSelContent() {
  std::string content = sel_content_;
  sel_content_ = "";
  return content;
}

int Complete::CalcMaxLines() {
  TermSize term_size = Terminal::Size();
  int size = term_size.lines - prompt_.NumOfLines();
  return size/2;
}

void Complete::PrintMoreOpt(bool selected) {
  has_more_ = true;

  if (selected) {
    std::cout << "\e[48;5;7m" << "[[more]]" << "\033[0m";
  } else {
    std::cout << "[[more]]";
  }
}

}  // namespace readline
