#include "history.h"

#include <string>
#include <vector>
#include <list>

#include "log.h"

namespace readline {

History::History(size_t size)
    : size_(size)
    , hist_counter_(0)
    , hist_mode_(false)
    , search_counter_(0)
    , search_mode_(false) {}

void History::Push(const std::string& str_cmd) {
  if (list_.size() >= size_) {
    list_.pop_back();
  }

  list_.push_front(str_cmd);
}

bool History::SearchQuery(const std::string& str_cmd) {
  LOG << "SearchQuery:" << str_cmd << "\n";
  if (IsInSearchMode()) {
    search_query_.clear();
    search_counter_ = 0;
  }

  for (const auto& e : list_) {
    if (e.find(str_cmd) != std::string::npos) {
      search_query_.push_back(e);
      LOG << "search_query_.push_back:" << e << "\n";
    }
  }

  if (search_query_.size() > 0) {
    search_mode_ = true;
    return true;
  }

  return false;
}

const std::string& History::GetSearchResult() {
  if (search_counter_ >= search_query_.size()) {
    search_counter_ = 0;
  }

  LOG << "search_query_.at:" << search_query_.at(search_counter_) << "\n";
  return search_query_.at(search_counter_++);
}

const std::string& History::GetNext() {
  hist_mode_ = true;

  if (hist_counter_ >= list_.size()) {
    hist_counter_ = 0;
  }

  auto it = list_.begin();

  for (size_t i = 0; i < hist_counter_; i++) {
    it++;
  }

  LOG << "List size:" << list_.size() << "\n";
  LOG << "GetNext hist_counter_:" << hist_counter_ << "\n";

  ++hist_counter_;

  return *it;
}

const std::string& History::GetPreviews() {
  auto it = list_.begin();

  if (hist_counter_ == 0) {
    hist_counter_ = 0;
  } else {
    if (hist_counter_ >= list_.size()) {
      --hist_counter_;
    }
    --hist_counter_;
  }

  LOG << "GetPreviews hist_counter_:" << hist_counter_ << "\n";

  for (size_t i = 0; i < hist_counter_; i++) {
    it++;
  }

  return *it;
}

bool History::HasSearchResult() {
  return search_query_.size() > 0;
}

bool History::HasNext() {
  if (hist_counter_ >= list_.size()) {
    return false;
  }

  return true;
}

bool History::HasPreviews() {
  if (hist_counter_ == 0) {
    return false;
  }

  return true;
}

void History::ExitHistMode() {
  hist_mode_ = false;
  hist_counter_ = 0;
}

bool History::IsInHistMode() {
  LOG << "IsInHistMode:" << hist_mode_ << "\n";
  return hist_mode_;
}

bool History::IsInSearchMode() {
  return search_query_.size() > 0;
}

void History::ExitSearchMode() {
  search_query_.clear();
  search_counter_ = 0;
  search_mode_ = false;
}

const std::list<std::string>& History::GetList() const {
  return list_;
}

size_t History::Count() const {
  return list_.size();
}

}
