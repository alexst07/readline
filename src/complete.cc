#include "complete.h"

#include <string>
#include <functional>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "cursor.h"
#include "prompt.h"

namespace readline {

Complete::Complete(int start_line, FuncComplete&& fn, Prompt& prompt)
    : start_line_(start_line)
    , item_sel_(0)
    , fn_complete_(std::move(fn))
    , complete_token_(false)
    , cursor_(prompt.GetCursorRef())
    , prompt_(prompt)
    , lines_show_(0)
    , show_(false) {}

void Complete::Show(const std::string& line, int line_pos) {
  if (show_) {
    Hide();
  }

  show_ = true;
  lines_show_ = Print(line, line_pos);
}

void Complete::Hide() {
  for (int i = 0; i <= lines_show_; i++) {
    cursor_.MoveToAbsolute(cursor_.GetStartLine() + i +1, 1);
    std::cout << "\033[K";
  }

  cursor_.MoveToPos(cursor_.GetPos());
  show_ = false;
}

int Complete::Print(const std::string& line, int line_pos) {
  std::vector<std::string> args = SplitArgs(line, line_pos);
  std::string last_arg = args.back();

  if (fn_complete_) {
    CompleteList list_result(fn_complete_(args));

    if (list_result.GetType() == CompleteList::Type::kList) {
      const std::vector<std::string>& list = boost::get<CompleteResultList>(
          list_result.GetItems()).GetItems();

      if (last_arg.empty()) {
        return PrintList(list);
      } else {
        // uses only item that match
        std::vector<std::string> new_list = MatchArg(last_arg, list);
        return PrintList(new_list);
      }
    }
  } else {
    std::vector<std::string> list = ListDir(".", ListDirType::FILES_DIR);

    if (last_arg.empty()) {
      return PrintList(list);
    } else {
      // uses only item that match
      std::vector<std::string> new_list = MatchArg(last_arg, list);
      return PrintList(new_list);
    }
  }
}

int Complete::PrintList(const std::vector<std::string>& list) {
  if (list.empty()) {
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
    std::cout << list[i];
  }

  cursor_.MoveToPos(cursor_.GetPos());
  return lines;
}

bool Complete::CheckNewArg(const std::string& line, int line_pos) {
  if (line_pos == 0) {
    return true;
  }

  if (line[line_pos - 1] == ' ') {
    return true;
  }

  return false;
}

std::vector<std::string> Complete::SplitArgs(const std::string& line,
    int line_pos) {
  std::vector<std::string> args;
  std::string sub = line.substr(0, line_pos);
  boost::split(args, sub, boost::algorithm::is_space());
  bool new_arg = CheckNewArg(line, line_pos);

  if (new_arg) {
    args.push_back("");
  }

  return args;
}

std::vector<std::string> Complete::MatchArg(const std::string& arg,
    std::vector<std::string> list) {
  std::vector<std::string> new_list;

  for (const auto& item: list) {
    if (item.find(arg) == 0) {
      new_list.push_back(item);
    }
  }

  return new_list;
}


std::vector<std::string> Complete::ListDir(const std::string& dir,
    ListDirType t) {
  namespace fs = boost::filesystem;

  std::vector<std::string> list;
  fs::path p(dir);
  fs::directory_iterator end_itr;

  for (fs::directory_iterator itr(p); itr != end_itr; ++itr) {
    if (t == ListDirType::FILES) {
      if (fs::is_regular_file(itr->path())) {
        std::string current_file = itr->path().string();
        list.push_back(current_file);
      }
    } else if (t == ListDirType::DIR) {
      if (fs::is_directory(itr->path())) {
        std::string current_file = itr->path().string();
        list.push_back(current_file);
      }
    } else {
      std::string current_file = itr->path().string();
      list.push_back(current_file);
    }
  }

  return list;
}

}  // namespace readline
