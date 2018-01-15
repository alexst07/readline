#ifndef READLINE_HISTORY_H
#define READLINE_HISTORY_H

#include <string>
#include <vector>
#include <list>

#include "complete.h"

namespace readline {

class History {
 public:
  History(size_t size);

  void Push(const std::string& str_cmd);

  bool SearchQuery(const std::string& str_cmd);

  const std::string& GetSearchResult();

  const std::string& GetPreviews();

  const std::string& GetNext();

  bool HasSearchResult();

  bool HasNext();

  bool HasPreviews();

  void ExitHistMode();

  bool IsInHistMode();

  bool IsInSearchMode();

  void ExitSearchMode();

  const std::list<std::string>& GetList() const;

  size_t Count() const;

  private:
   size_t size_;
   std::list<std::string> list_;
   std::vector<std::string> search_query_;
   size_t hist_counter_;
   bool hist_mode_;
   size_t search_counter_;
   bool search_mode_;
};

}  // namespace readline

#endif  // READLINE_HISTORY_H
