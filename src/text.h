#ifndef READLINE_TEXT_H
#define READLINE_TEXT_H

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <map>

namespace readline {

class Style {
 public:
  explicit Style(const std::string& str_style)
      : str_style_(str_style){}

  const std::string& str_style() const {
    return str_style_;
  }

 private:
  std::string str_style_;
};

class Text {
 public:
 explicit Text(const std::string& str)
     : str_(str)
     , str_with_style_(str) {}

  Text(const Text& text)
      : str_(text.str_)
      , str_with_style_(text.str_with_style_) {}

  Text& operator=(const Text& text) {
    str_ = text.str_;
    str_with_style_ = text.str_with_style_;

    return *this;
  }

  Text(Text&& text)
      : str_(std::move(text.str_))
      , str_with_style_(std::move(text.str_with_style_)) {}

  Text& operator=(Text&& text) {
    str_ = std::move(text.str_);
    str_with_style_ = std::move(text.str_with_style_);

    return *this;
  }

  const std::string& StrWithStyle() const {
    return str_with_style_;
  }

  const std::string& Value() const {
    return str_;
  }

  size_t Length() const {
    return str_.length();
  }

  Text& operator<< (const std::string& val) {
    str_ += val;
    str_with_style_ += val;

    return *this;
  }

  Text& operator<< (const Style& val_style) {
    str_with_style_ += val_style.str_style();

    return *this;
  }

 private:
  std::string str_;
  std::string str_with_style_;
};

class Item {
 public:
  Item(const std::string& value, Text&& text)
      : value_(value)
      , text_(std::move(text)) {}

  Item(const Item& item)
      : value_(item.value_)
      , text_(item.text_) {}

  Item& operator=(const Item& item) {
    value_ = item.value_;
    text_ = item.text_;

    return *this;
  }

  Item(Item&& item)
      : value_(std::move(item.value_))
      , text_(std::move(item.text_)) {}

  Item& operator=(Item&& item) {
    value_ = std::move(item.value_);
    text_ = std::move(item.text_);

    return *this;
  }

  void Print() const {
    std::cout << text_.StrWithStyle();
  }

  const std::string& StrWithStyle() const {
    return text_.StrWithStyle();
  }

  const std::string& Value() const {
    return value_;
  }

 private:
  std::string value_;
  Text text_;
};

struct ItemDescr {
 public:
 ItemDescr(const std::string& value, Text&& label, Text&& desrc)
     : value_(value)
     , label_(std::move(label))
     , descr_(std::move(desrc)) {}

 ItemDescr(const ItemDescr& item)
     : value_(item.value_)
     , label_(item.label_)
     , descr_(item.descr_) {}

 ItemDescr& operator=(const ItemDescr& item) {
   value_ = item.value_;
   label_ = item.label_;
   descr_ = item.descr_;

   return *this;
 }

 ItemDescr(ItemDescr&& item)
     : value_(std::move(item.value_))
     , label_(std::move(item.label_))
     , descr_(std::move(item.descr_)) {}

 ItemDescr& operator=(ItemDescr&& item) {
   value_ = std::move(item.value_);
   label_ = std::move(item.label_);
   descr_ = std::move(item.descr_);

   return *this;
 }

 void Print(size_t num_cols, size_t size_first_col) const {

 }

 const std::string& Value() const {
   return value_;
 }

 private:
  std::string value_;
  Text label_;
  Text descr_;
};

class List {
 public:
  virtual void Print(size_t i, size_t num_cols) const = 0;

  virtual const std::string& Value(size_t i) const = 0;

  virtual const std::string& StrWithStyle(size_t i) const = 0;

  virtual size_t Size() const = 0;

  virtual void EraseIf(std::function<bool(const std::string&)>&& fn) = 0;

  virtual bool Empty() const = 0;

  virtual size_t MaxStringLen() const = 0;
};

class ListItem: public List {
 public:
  explicit ListItem(const std::vector<std::string>& list) {
    for (const auto& e : list) {
      items_.push_back(Item(e, Text(e)));
    }
  }

  explicit ListItem(
      const std::vector<std::pair<std::string, std::string>>& list) {
    for (const auto& e : list) {
      items_.push_back(Item(e.first, Text(e.second)));
    }
  }

  explicit ListItem(std::vector<Item>&& items)
      : items_(std::move(items)) {}

  ListItem& operator=(std::vector<std::string>&& list) {
    for (const auto& e : list) {
      items_.push_back(Item(e, Text(e)));
    }

    return *this;
  }

  void Print(size_t i, size_t num_cols) const override {
    items_[i].Print();
  }

  const std::string& Value(size_t i) const override {
    return items_[i].Value();
  }

  const std::string& StrWithStyle(size_t i) const override {
    return items_[i].Value();
  }

  size_t Size() const override {
    return items_.size();
  }

  void EraseIf(std::function<bool(const std::string&)>&& fn) override {
    items_.erase(std::remove_if(items_.begin(), items_.end(),
        [&](Item& i) { return fn(i.Value()); }), items_.end());
  }

  bool Empty() const override {
    return items_.empty();
  }

  size_t MaxStringLen() const override {
    size_t len = 0;

    for (const auto& item: items_) {
      size_t str_len = item.StrWithStyle().length();
      if (str_len > len) {
        len = str_len;
      }
    }

    return len;
  }

 private:
  std::vector<Item> items_;
};

// class ListDescr: public List {
//
// };

}  // namespace readline

#endif  // READLINE_TEXT_H
