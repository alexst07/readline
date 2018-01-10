#include "utils.h"

#include <memory>
#include <vector>
#include <algorithm>
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

  if (args.size() > 0) {
    if (args.back().empty()) {
      new_arg = false;
    }
  }

  // if the last arg is empty, insert empty string
  if (new_arg) {
    args.push_back("");
  }

  if (args.size() > 2) {
    if (args[args.size() - 1].empty() && args[args.size() - 2].empty()) {
      args.pop_back();
    }
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

  std::sort(list.begin(), list.end());
  return list;
}

void MatchArg(const std::string& arg, List* list) {
  list->EraseIf([&](const std::string& item) {
    if (item.find(arg) != 0) {
      return true;
    }

    return false;
  });
}

std::vector<std::string> MatchArg(const std::string& arg,
    std::vector<std::string>& list) {
  std::vector<std::string> new_list;

  for (const auto& item: list) {
    if (item.find(arg) == 0) {
      new_list.push_back(item);
    }
  }

  return new_list;
 }

std::vector<ItemDescr> MatchArg(const std::string& arg,
     std::vector<ItemDescr>& list) {
   std::vector<ItemDescr> new_list;

   for (const auto& item: list) {
     if (item.Value().find(arg) == 0) {
       new_list.push_back(item);
     }
   }

   return new_list;
  }


 std::unique_ptr<List> MatchDirList(const std::vector<std::string>& args) {
   // if it is a new arg, the last arg is empty
   std::string last_arg = args.back();

   std::string path;
   std::string last_part;
   std::tie(path, last_part) = ParserPath(last_arg);

   // on this part we can be certain, that it is a simple list and not
   // a descr list, so we can use static cast
   std::unique_ptr<List> items_ = std::make_unique<ListItem>(
      ListDir(path, ListDirType::FILES_DIR));

   if (last_part.empty()) {
     return items_;
   } else {
     // uses only item that match
     MatchArg(last_part, items_.get());
     return items_;
   }
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

std::tuple<std::unique_ptr<List>, RetType, bool> RetDirFileList(
      const std::vector<std::string>& params, bool tip, ListDirType type) {
  RetType ret_type = RetType::FILES_DIR;
  bool is_path = false;

  std::string last_arg = params.back();

  std::string path;
  std::string last_part;
  std::tie(path, last_part) = ParserPath(last_arg);

  if (IsDirectory(path)) {
    is_path = true;
    if (tip) {
      auto items = ListDir(path, type);

      if (!last_part.empty()) {
        // uses only item that match
        items = MatchArg(last_part, items);
      }

      return std::tuple<std::unique_ptr<List>, RetType, bool>(
          std::unique_ptr<List>(new ListItem(items)), ret_type, is_path);
    } else {
      return std::tuple<std::unique_ptr<List>, RetType, bool>(
          MatchDirList(params), ret_type, is_path);
    }
  }
}

std::tuple<std::unique_ptr<List>, RetType, bool> RetList(
    std::vector<std::string>&& plist, const std::vector<std::string>& params,
    bool tip) {
  std::vector<std::string> list = std::move(plist);

  if (tip) {
    if (!params.back().empty()) {
      // uses only item that match
      list = MatchArg(params.back(), list);
    }
  }

  return std::tuple<std::unique_ptr<List>, RetType, bool>(
      std::unique_ptr<List>(new ListItem(list)), RetType::LIST, false);
}

std::tuple<std::unique_ptr<List>, RetType, bool> RetList(
    std::vector<ItemDescr>&& plist_descr,
    const std::vector<std::string>& params, bool tip) {
  std::vector<ItemDescr> list_descr = std::move(plist_descr);

  if (tip) {
    if (!params.back().empty()) {
      // uses only item that match
      list_descr = MatchArg(params.back(), list_descr);
    }
  }

  return std::tuple<std::unique_ptr<List>, RetType, bool>(
      std::unique_ptr<List>(new ListDescr(std::move(list_descr))),
          RetType::DESCR, false);
}

std::wstring str2wstr(const std::string& str) {
  std::wstring ws_tmp(str.begin(), str.end());
  return ws_tmp;
}

std::string wstr2str(const std::wstring& wstr) {
  std::string tmp(wstr.begin(), wstr.end());
  return tmp;
}

}
