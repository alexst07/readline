#include "complete.h"

#include <string>
#include <functional>
#include <vector>
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
    , show_always_(false) {}

void Complete::Show(const std::vector<std::string>& args, bool show_always) {
  if (show_) {
    Hide();
  }

  show_ = true;
  show_always_ = show_always;
  lines_show_ = Print(args);
}

void Complete::Show() {
  if (show_) {
    CleanLines();
  }

  PrintList(items_);
}

void Complete::Hide() {
  CleanLines();
  show_ = false;
  item_sel_ = -1;
}

void Complete::CleanLines() {
  for (int i = 0; i <= lines_show_; i++) {
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + i +1, 1);
    std::cout << "\033[K";
  }

  cursor_.MoveToPos(cursor_.GetPos());
}

int Complete::Print(const std::vector<std::string>& args) {
  std::string last_arg = args.back();

  if (fn_complete_) {
    CompleteList list_result(fn_complete_(args));

    if (list_result.GetType() == CompleteList::Type::kList) {
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
    items_ = ListDir(".", ListDirType::FILES_DIR);

    if (last_arg.empty()) {
      return PrintList(items_);
    } else {
      // uses only item that match
      items_ = MatchArg(last_arg, items_);
      return PrintList(items_);
    }
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
  int ncols = term_size.cols / len;

  if (ncols > 1) {
    // calcualte the new ncols, at this time we multiply 3*ncols, because
    // this is the amount of white spaces to separate the items
    ncols = (term_size.cols - 3*ncols)/len;
    return PrintItemsList(list, ncols, len);
  } else {
    int lines = 0;

    for (size_t i = 0; i < list.size(); i++) {;
      cursor_.MoveToAbsolute(
          cursor_.GetStartLine() + prompt_.NumOfLines()+lines, 1);
      std::cout << list[i];
      ++lines;
    }

    cursor_.MoveToPos(cursor_.GetPos());
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

  if (pos_line + num_lines > term_size.lines) {
    int num_add_lines = term_size.lines - pos_line + num_lines;
    prompt_.AddLines(num_add_lines);
  }

  for (size_t i = 0; i < list.size(); i++) {
    if (i%nc == 0 && i > 0) {
      ++lines;
    }

    // show the items side by side, I multiply i by 2 to keep two spaces
    // between each item
    int mod = i%nc;
    int pos_col = mod* len + (mod > 0? mod*2: 0);
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

  cursor_.MoveToPos(cursor_.GetPos());
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

  int sel = item_sel_ + 1;

  if (sel >= total_items_) {
    item_sel_ = 0;
    return;
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
  auto cleanup = MakeScopeExit([&]() {
    Show();
  });
  IgnoreUnused(cleanup);

  if (item_sel_ < 0) {
    item_sel_ = 0;
    return;
  }

  int sel = item_sel_ + num_cols_;

  if (sel == total_items_ - 1) {
    item_sel_ = 0;
    return;
  }

  if (sel >= total_items_) {
    item_sel_ = total_items_ - 1;
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
  }

  int sel = item_sel_ - num_cols_;

  if (item_sel_ == 0) {
    item_sel_ = total_items_ - 1;
    return;
  }

  if (sel < 0) {
    item_sel_ = 0;
    return;
  }

  item_sel_ -= num_cols_;
}

std::string Complete::UseSelContent() {
  std::string content = sel_content_;
  sel_content_ = "";
  return content;
}

}  // namespace readline
