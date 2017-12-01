#ifndef READLINE_COMPLETE_H
#define READLINE_COMPLETE_H

#include <string>
#include <functional>
#include <vector>
#include <boost/variant.hpp>
#include "cursor.h"
#include "prompt.h"

namespace readline {

class CompleteResultFiles {};
class CompleteResultDirs {};
class CompleteResultDirsAndFiles {};

class CompleteResultList {
  CompleteResultList(std::vector<std::string>&& items)
      : items_(std::move(items)) {}

 private:
  std::vector<std::string> items_;
};

class CompleteResultListDescr {
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

class Complete {
 public:
  using FuncType =
      CompleteList(const std::vector<std::string>& params, bool new_param);

  Complete(const std::string& line, int line_pos, int start_line,
      FuncType&& fn, Cursor& cursor, Prompt& prompt);

  void Show();

  void Hide();

  void SelNextItem();

  void SelBackItem();

  void SelDownItem();

  void SelUpItem();

 private:
  void Print();

  void PrintList(const std::vector<std::string>& list);

  void PrintItemsList(const std::vector<std::string>& list, int nc, int len);

  void PrintListDescr(
      const std::vector<std::pair<std::string, std::string>>& list);

  bool CheckNewArg();

  enum class ListDirType {
    FILES,
    DIR,
    FILES_DIR
  };

  std::vector<std::string> ListDir(const std::string& dir, ListDirType t);

  std::string line_;
  int line_pos_;
  int start_line_;
  int item_sel_;
  std::string complete_;
  std::function<FuncType> fn_complete_;
  bool complete_token_;
  Cursor& cursor_;
  Prompt& prompt_;
};

}  // namespace readline

#endif  // READLINE_COMPLETE_H
