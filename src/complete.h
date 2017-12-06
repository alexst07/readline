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
  enum Select {
    kMore = -2
  };

  Complete(int start_line, FuncComplete&& fn, Prompt& prompt);

  void Show(const std::vector<std::string>& args, bool show_always = false);

  void Show();

  void Hide();

  void CompleteTip(const std::vector<std::string>& args);

  void CleanLines();

  void SelNextItem();

  void SelBackItem();

  void SelDownItem();

  void SelUpItem();

  std::string UseSelContent();

  void SelectFirstItem();

  int ItemSelected() const {
    return item_sel_;
  }

  bool IsAnyItemSelected() const {
    return item_sel_ >= 0? true:false;
  }

  inline bool Showing() const {
    return show_;
  }

  inline int ListSize() const {
    return items_.size();
  }

  inline bool IsPathComplete() const {
    return is_path_;
  }

 private:
  int Print(const std::vector<std::string>& args);

  int PrintList(const std::vector<std::string>& list);

  int PrintItemsList(const std::vector<std::string>& list, int nc, int len);

  void PrintListDescr(
      const std::vector<std::pair<std::string, std::string>>& list);

  int CalcMaxLines();

  void PrintMoreOpt(bool selected);

  int start_line_;
  int item_sel_;
  std::string complete_;
  FuncComplete fn_complete_;
  bool complete_token_;
  Cursor& cursor_;
  Prompt& prompt_;

  // how many lines was used in last show operation
  int lines_show_;

  // a flag to indicate if the complete menu is being showed
  bool show_;

  // string content of last selections
  std::string sel_content_;

  // numbers of cols of last show operation
  int num_cols_;

  // total items in list of last show operation
  int total_items_;

  // items of last show operation
  std::vector<std::string> items_;

  // if the list must be showed, even when there is only one element
  bool show_always_;

  // flat for sinlize if the las complete list was for directory
  bool is_path_;

  // flag to sinalize that has more items than tha lines allowed to print
  bool has_more_;
};

}  // namespace readline

#endif  // READLINE_COMPLETE_H
