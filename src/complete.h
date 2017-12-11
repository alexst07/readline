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

enum class RetType {
  FILES_DIR,
  LIST,
  DESCR
};

using FuncComplete = std::function<std::tuple<std::unique_ptr<List>, RetType,
    bool>(const std::vector<std::string>&, bool)>;

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
