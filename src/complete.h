#ifndef READLINE_COMPLETE_H
#define READLINE_COMPLETE_H

#include <string>
#include <functional>
#include <vector>
#include <boost/variant.hpp>
#include "cursor.h"
#include "text.h"

namespace readline {
class Prompt;

class CompleteResult {
 public:
  enum class Type {
    kFiles,
    kDirs,
    kDirsAndFiles,
    kList,
    kListDescr
  };

  CompleteResult(Type type): type_(type) {}

  Type GetType() {
    return type_;
  }

 private:
  Type type_;
};

class CompleteResultFiles: public CompleteResult {
 public:
  CompleteResultFiles(): CompleteResult(CompleteResult::Type::kFiles) {}
};

class CompleteResultDirs: public CompleteResult {
 public:
  CompleteResultDirs(): CompleteResult(CompleteResult::Type::kDirs) {}
};

class CompleteResultDirsAndFiles: public CompleteResult {
 public:
  CompleteResultDirsAndFiles()
      : CompleteResult(CompleteResult::Type::kDirsAndFiles) {}
};

class CompleteResultList: public CompleteResult {
 public:
  CompleteResultList(std::vector<std::string>& items)
      : CompleteResult(CompleteResult::Type::kList)
      , items_(std::move(std::make_unique<ListItem>(items))) {}

  std::unique_ptr<List> GetItemsPtr() {
    return std::move(items_);
  }

 private:
  std::unique_ptr<List> items_;
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
  using Type = CompleteResult::Type;

  CompleteList(std::unique_ptr<CompleteResult> result_list)
      : result_list_(std::move(result_list)) {}

  Type GetType() {
    return result_list_->GetType();
  }

 private:
  std::unique_ptr<CompleteResult> result_list_;
};

using FuncComplete =
    std::function<CompleteList(const std::vector<std::string>&)>;

class Complete {
 public:
  enum Select {
    kMore = -2
  };

  Complete(FuncComplete&& fn, Prompt& prompt);

  void Show(const std::vector<std::string>& args, bool show_always = false);

  void Show();

  void Hide();

  void CompleteTip(const std::vector<std::string>& args);

  void CleanLines();

  void SelNextItem();

  void SelBackItem();

  void SelDownItem();

  void SelUpItem();

  void SelCheck();

  int FullScreenMenu();

  void PrintAllItems();

  void FullScreenMenuWithBar(int menu_size);

  int FullScreenTotalItems(int menu_size);

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
    return items_->Size();
  }

  inline bool IsPathComplete() const {
    return is_path_;
  }

 private:
  int Print(const std::vector<std::string>& args);

  int PrintList(List& list);

  int PrintItemsList(List& list);

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
  std::unique_ptr<List> items_;

  // if the list must be showed, even when there is only one element
  bool show_always_;

  // flat for sinlize if the las complete list was for directory
  bool is_path_;

  // flag to sinalize that has more items than tha lines allowed to print
  bool has_more_;

  bool full_screen_mode_;

  int full_screen_line_;

  int max_string_len_;

  bool all_items_mode_;
};

}  // namespace readline

#endif  // READLINE_COMPLETE_H
