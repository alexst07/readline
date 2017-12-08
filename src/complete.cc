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

Complete::Complete(FuncComplete&& fn, Prompt& prompt)
    : item_sel_(-1)
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
    , full_screen_mode_(false)
    , full_screen_line_(0)
    , max_string_len_(0)
    , all_items_mode_(false) {}

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
    max_string_len_ = 0;
    has_more_ = false;
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
  max_string_len_ = 0;
  full_screen_mode_ = false;
  all_items_mode_ = false;
  num_cols_ = 0;
}

void Complete::CleanLines() {
  for (int i = 0; i <= lines_show_; i++) {
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + prompt_.NumOfLines() + i,
        1);
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
  // get the max string len item
  size_t len = 0;
  if (max_string_len_ == 0) {
    for (const auto& item: list) {
      if (item.length() > len) {
        len = item.length();
      }
    }

    max_string_len_ = len;
  }

  TermSize term_size = Terminal::Size();

  // we add by 3, because we want 3 white space between each item
  num_cols_ = term_size.cols / (max_string_len_ + 3);

  // choose the correct print mode
  if (full_screen_mode_ || all_items_mode_) {
    return FullScreenMenu();
  }

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

  if (num_cols_ > 1) {
    return PrintItemsList(list);
  } else {
    num_cols_ = 1;
    return PrintItemsList(list);
  }
}

int Complete::PrintItemsList(const std::vector<std::string>& list) {
  int lines = 0;
  total_items_ = list.size();

  // calculates the number of needed lines
  int num_lines = static_cast<int>(std::ceil(
      static_cast<float>(list.size()) / static_cast<float>(num_cols_)));

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
    if (i%num_cols_ == 0 && i > 0) {
      ++lines;
      cursor_.MoveToAbsolute(
          cursor_.GetStartLine() + prompt_.NumOfLines()+lines, 1);
      // clean the line
      std::cout << "\033[K";

      if (lines >= CalcMaxLines()) {
        // print more option
        PrintMoreOpt(item_sel_ == kMore);
        return lines;
      }
    }

    // show the items side by side, I multiply i by 3 to keep two spaces
    // between each item
    int mod = i%num_cols_;
    int pos_col = mod* max_string_len_ + (mod > 0? mod*3: 0);
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

void Complete::PrintAllItems() {
  int lines = 0;

  for (size_t i = 0; i < items_.size(); i++) {
    if (i%num_cols_ == 0 && i > 0) {
      ++lines;
    }

    int mod = i%num_cols_;
    int pos_col = mod* max_string_len_ + (mod > 0? mod*3: 0);
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + prompt_.NumOfLines()
        +lines, pos_col + 1);

    if (i == item_sel_) {
      sel_content_ = items_[i];
      std::cout << "\e[48;5;7m" << items_[i] << "\033[0m";
      prompt_.ShowTip(items_[i]);
    } else {
      std::cout << items_[i];
    }
  }
}

int Complete::FullScreenMenu() {
  has_more_ = false;

  // calculate the size of menu
  TermSize term_size = Terminal::Size();
  int pos_line = prompt_.GetCursorRef().CalcAbsoluteLine(
      prompt_.Str().length());

  // subtract 1, bacause last line is status line
  int menu_size = term_size.lines - prompt_.NumOfLines();

  // calculate how many lines we need to add
  if (pos_line + menu_size > term_size.lines) {
    int num_add_lines = menu_size - (term_size.lines - pos_line);
    prompt_.AddLines(num_add_lines);
  }

  // clean all lines
  for (size_t i = 0; i < menu_size; i++) {
    cursor_.MoveToAbsolute(
        cursor_.GetStartLine() + prompt_.NumOfLines()+ i + 1, 1);
    // clean the line
    std::cout << "\033[K";
  }

  // calculate if we can show all items in the screen
  int num_lines = static_cast<int>(std::ceil(
      static_cast<float>(items_.size()) / static_cast<float>(num_cols_)));

  if (num_lines <= menu_size) {
    PrintAllItems();
    return menu_size;
  } else {
    FullScreenMenuWithBar(menu_size);
    return menu_size;
  }
}

int Complete::FullScreenTotalItems(int menu_size) {
  // calculate the end item of full screen
  int last_items = (full_screen_line_ + menu_size)*num_cols_;
  last_items = last_items >= items_.size()? items_.size() - 1:last_items;
  return last_items;
}

void Complete::FullScreenMenuWithBar(int menu_size) {
  full_screen_mode_ = true;
  int lines = 0;

  // calculates the last item given the size of menu
  // the size of menu is subtracted because the last line is for status
  int last_items = FullScreenTotalItems(menu_size - 1);

  // calculates the first item to show
  int start = full_screen_line_*num_cols_;

  for (size_t i = start; i < last_items; i++) {
    if (i%num_cols_ == 0 && i > start) {
      ++lines;
    }

    // calculate the position of each item
    int mod = i%num_cols_;
    int pos_col = mod* max_string_len_ + (mod > 0? mod*3: 0);

    cursor_.MoveToAbsolute(cursor_.GetStartLine() + prompt_.NumOfLines()
        +lines, pos_col + 1);

    if (i == item_sel_) {
      // print the selected item with different color, and update the tip
      sel_content_ = items_[i];
      std::cout << "\e[48;5;7m" << items_[i] << "\033[0m";
      prompt_.ShowTip(items_[i]);
    } else {
      std::cout << items_[i];
    }
  }

  // write status on last line
  cursor_.MoveToAbsolute(cursor_.GetStartLine() + prompt_.NumOfLines()
      + ++lines, 1);
  int total_lines = static_cast<int>(std::ceil(
      static_cast<float>(total_items_ - 1) / static_cast<float>(num_cols_)));

  std::cout << "\e[44m\e[97m" << "rows " << full_screen_line_ + 1
            << " to " << full_screen_line_ + menu_size - 1
            << " from " << total_lines << "\033[0m";
}

void Complete::SelCheck() {
  if (full_screen_mode_) {
    // calculate the size of menu
    TermSize term_size = Terminal::Size();
    int menu_size = term_size.lines - prompt_.NumOfLines() - 1;

    int last_item = FullScreenTotalItems(menu_size);
    int first_item = full_screen_line_*num_cols_;

    if (item_sel_ >= last_item) {
      int term = static_cast<int>(std::ceil(
          static_cast<float>(item_sel_) / static_cast<float>(num_cols_)));

      // for first collumn we have to add 1, so we have to check if it is
      // the first collumn
      full_screen_line_ = term - menu_size + (item_sel_%num_cols_ == 0?1:0);
    }

    if (item_sel_ < first_item) {
      full_screen_line_ = item_sel_/num_cols_;
    }
  }

  Show();
}

void Complete::SelectFirstItem() {
  auto cleanup = MakeScopeExit([&]() {
    SelCheck();
  });
  IgnoreUnused(cleanup);

  item_sel_ = 0;
}

void Complete::SelNextItem() {
  auto cleanup = MakeScopeExit([&]() {
    SelCheck();
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
    SelCheck();
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
    SelCheck();
  });
  IgnoreUnused(show);

  if (item_sel_ < 0) {
    item_sel_ = 0;
    return;
  }

  int total;
  if (has_more_) {
    // TODO: I didn't understand why lines_show_ works
    // and lines_show_ - 1 not work
    total = (lines_show_)*num_cols_;
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
    SelCheck();
  });
  IgnoreUnused(cleanup);

  if (item_sel_ < 0) {
    item_sel_ = 0;
    return;
  }

  // calculate the selection of the preview item
  int sel = item_sel_ - num_cols_;

  if (item_sel_ == 0) {
    item_sel_ = total_items_ - 1;
    return;
  }

  if (sel < 0) {
    // calculate the first item from preview column
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
    // select first item
    item_sel_ = 0;
    FullScreenMenu();
  } else {
    std::cout << "[[more]]";
  }
}

}  // namespace readline
