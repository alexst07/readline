#ifndef READLINE_COMPLETE_H
#define READLINE_COMPLETE_H

#include <string>
#include <functional>
#include <vector>
#include <boost/variant.hpp>
#include "cursor.h"

namespace readline {
class Prompt;

class CompleteResultFiles {};
class CompleteResultDirs {};
class CompleteResultDirsAndFiles {};

class CompleteResultList {
 public:
  CompleteResultList(std::vector<std::string>&& items)
      : items_(std::move(items)) {}

  const std::vector<std::string> GetItems() const {
    return items_;
  }

 private:
  std::vector<std::string> items_;
};

class CompleteResultListDescr {
 public:
  CompleteResultListDescr(
      std::vector<std::pair<std::string, std::string>>&& items)
      : items_(std::move(items)) {}

 private:
  std::vector<std::pair<std::string, std::string>> items_;
};

class CompleteList {
 public:
  using Variant = boost::variant<CompleteResultFiles, CompleteResultDirs,
     CompleteResultDirsAndFiles, CompleteResultList,
     CompleteResultListDescr>;

  enum class Type {
    kFiles,
    kDirs,
    kDirsAndFiles,
    kList,
    kListDescr
  };

  CompleteList(CompleteResultFiles&& e)
      : type_(Type::kFiles)
      , items_(std::move(e)) {}

  CompleteList(CompleteResultDirs&& e)
      : type_(Type::kDirs)
      , items_(std::move(e)) {}

  CompleteList(CompleteResultDirsAndFiles&& e)
      : type_(Type::kDirsAndFiles)
      , items_(std::move(e)) {}

  CompleteList(CompleteResultList&& e)
      : type_(Type::kList)
      , items_(std::move(e)) {}

  CompleteList(CompleteResultListDescr&& e)
      : type_(Type::kListDescr)
      , items_(std::move(e)) {}

  Type GetType() const {
    return type_;
  }

  Variant& GetItems() {
    return items_;
  }

 private:
   Type type_;
   Variant items_;
};

using FuncComplete =
    std::function<CompleteList(const std::vector<std::string>&)>;

class Complete {
 public:
  Complete(int start_line, FuncComplete&& fn, Prompt& prompt);

  void Show(const std::string& line, int line_pos);

  void Hide();

  void SelNextItem();

  void SelBackItem();

  void SelDownItem();

  void SelUpItem();

  inline bool Showing() const {
    return show_;
  }

 private:
  int Print(const std::string& line, int line_pos);

  int PrintList(const std::vector<std::string>& list);

  int PrintItemsList(const std::vector<std::string>& list, int nc, int len);

  void PrintListDescr(
      const std::vector<std::pair<std::string, std::string>>& list);

  bool CheckNewArg(const std::string& line, int line_pos);

  std::vector<std::string> SplitArgs(const std::string& line, int line_pos);

  std::vector<std::string> MatchArg(const std::string& arg,
      std::vector<std::string> list);

  enum class ListDirType {
    FILES,
    DIR,
    FILES_DIR
  };

  std::vector<std::string> ListDir(const std::string& dir, ListDirType t);

  int start_line_;
  int item_sel_;
  std::string complete_;
  FuncComplete fn_complete_;
  bool complete_token_;
  Cursor& cursor_;
  Prompt& prompt_;
  int lines_show_;
  bool show_;
};

}  // namespace readline

#endif  // READLINE_COMPLETE_H
