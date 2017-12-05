#include "utils.h"

#include <memory>
#include <vector>
#include <string>
#include <tuple>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>


namespace readline {

std::vector<std::string> SplitArgs(const std::string& line,
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

bool CheckNewArg(const std::string& line, int line_pos) {
  if (line_pos == 0) {
    return true;
  }

  if (line[line_pos - 1] == ' ') {
    return true;
  }

  return false;
}

std::vector<std::string> ListDir(const std::string& dir, ListDirType t) {
  namespace fs = boost::filesystem;

  std::vector<std::string> list;
  fs::path p(dir);
  fs::directory_iterator end_itr;

  for (fs::directory_iterator itr(p); itr != end_itr; ++itr) {
    std::string current_file = itr->path().string();
    size_t last_bar_pos = current_file.find_last_of("/");
    current_file = current_file.substr(last_bar_pos + 1);

    if (t == ListDirType::FILES) {
      if (fs::is_regular_file(itr->path())) {
        list.push_back(current_file);
      }
    } else if (t == ListDirType::DIR) {
      if (fs::is_directory(itr->path())) {
        list.push_back(current_file);
      }
    } else {
      list.push_back(current_file);
    }
  }

  return list;
}

std::vector<std::string> MatchArg(const std::string& arg,
    std::vector<std::string> list) {
  std::vector<std::string> new_list;

  for (const auto& item: list) {
    if (item.find(arg) == 0) {
      new_list.push_back(item);
    }
  }

  return new_list;
}

std::tuple<std::string, std::string> ParserPath(const std::string& arg,
    bool supress_point) {
  namespace fs = boost::filesystem;
  std::vector<std::string> elements;
  std::string path = "";
  std::string last;

  std::string path_arg = arg == ""? ".":arg;

  for(auto& part : fs::path(path_arg)) {
    elements.push_back(part.filename().string());
  }

  if (!(elements.size() == 1 && (elements[0] == "/" || elements[0] == "."))) {
    last = elements.back();
    elements.pop_back();
  }

  for (auto& e : elements) {
    if (e != "/") {
      path += e + "/";
    } else {
      path += e;
    }
  }

  if (last == "." && supress_point) {
    last = "";
  }

  if (path == "") {
    path = ".";
  } else if (path.substr(0,2) == "~/") {
    boost::replace_first(path, "~/", std::string(getenv("HOME")) + "/");
  }

  return std::tuple<std::string, std::string>(path, last);
}

std::string DirectoryFormat(const std::string& dir) {
  if (IsDirectory(dir)) {
    return dir + "/";
  } else {
    return dir;
  }
}

bool IsDirectory(const std::string path) {
  namespace fs = boost::filesystem;
  std::string str_path = path;

  if (path.substr(0,2) == "~/") {
    boost::replace_first(str_path, "~/", std::string(getenv("HOME")) + "/");
  }

  if (fs::is_directory(fs::path(str_path))) {
    return true;
  }

  return false;
}

}
