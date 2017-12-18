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
  Text() = default;

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

  friend Text operator+ (const Text& text, const std::string& val);

  Text& operator<< (const std::string& val) {
    str_ += val;
    str_with_style_ += val;

    return *this;
  }

  Text& operator<< (const Style& val_style) {
    str_with_style_ += val_style.str_style();

    return *this;
  }

  operator std::string() const {
    return str_with_style_;
  }

 private:
  std::string str_;
  std::string str_with_style_;
};

inline Text operator+(const Text& text, const std::string& val) {
  Text t;
  t.str_ = text.str_;
  t.str_with_style_ = text.str_with_style_;
  t << val;

  return t;
}

inline std::ostream& operator<<(std::ostream& stream, const Text& text) {
  stream  << text.StrWithStyle();
  return stream;
}

class Item {
 public:
  explicit Item(Text&& text)
      : text_(std::move(text)) {}

  explicit Item(const Text& text)
      : text_(text) {}

  explicit Item(const std::string& str)
      : text_(str) {}

  Item(const Item& item)
      : text_(item.text_) {}

  Item& operator=(const Item& item) {
    text_ = item.text_;

    return *this;
  }

  Item(Item&& item)
      : text_(std::move(item.text_)) {}

  Item& operator=(Item&& item) {
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
    return text_.Value();
  }

 private:
  Text text_;
};

struct ItemDescr {
 public:
 ItemDescr(const std::string& value, Text&& label, Text&& desrc)
     : label_(std::move(label))
     , descr_(std::move(desrc)) {}

 ItemDescr(const ItemDescr& item)
     : label_(item.label_)
     , descr_(item.descr_) {}

 ItemDescr& operator=(const ItemDescr& item) {
   label_ = item.label_;
   descr_ = item.descr_;

   return *this;
 }

 ItemDescr(ItemDescr&& item)
     : label_(std::move(item.label_))
     , descr_(std::move(item.descr_)) {}

 ItemDescr& operator=(ItemDescr&& item) {
   label_ = std::move(item.label_);
   descr_ = std::move(item.descr_);

   return *this;
 }

 void Print(size_t num_cols, size_t size_first_col) const {
   std::string spaces = "";
   for (int i = 0; i < (size_first_col - label_.Length()); i++) {
     spaces += " ";
   }

   size_t max_string_len = num_cols - size_first_col - 2;
   std::string descr;
   if (descr_.Length() > max_string_len) {
     descr = descr_.StrWithStyle().substr(size_first_col + 2,
       max_string_len - 3);
     descr += "...";
   } else {
     descr = descr_.StrWithStyle();
   }

   std::cout << label_ << spaces << ": " << descr;
 }

 size_t LabelLength() const {
   return label_.Length();
 }

 size_t DescrLength() const {
   return descr_.Length();
 }

 const std::string& LabelWithStyle() const {
   return label_.StrWithStyle();
 }

 const std::string& DescrWithStyle() const {
   return descr_.StrWithStyle();
 }

 const std::string& Value() const {
   return label_.Value();
 }

 private:
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
  ListItem() = default;

  explicit ListItem(const std::vector<std::string>& list) {
    for (const auto& e : list) {
      items_.push_back(Item(Text(e)));
    }
  }

  explicit ListItem(const std::vector<Text>& list) {
    for (const auto& e : list) {
      items_.push_back(Item(e));
    }
  }

  explicit ListItem(std::vector<Item>&& items)
      : items_(std::move(items)) {}

  ListItem& operator=(std::vector<std::string>&& list) {
    for (const auto& e : list) {
      items_.push_back(Item(Text(e)));
    }

    return *this;
  }

  void Print(size_t i, size_t /*num_cols*/) const override {
    items_[i].Print();
  }

  const std::string& Value(size_t i) const override {
    return items_[i].Value();
  }

  const std::string& StrWithStyle(size_t i) const override {
    return items_[i].StrWithStyle();
  }

  size_t Size() const override {
    return items_.size();
  }

  void PushBack(const Text& text) {
    items_.push_back(Item(text));
  }

  void PushBack(Text&& text) {
    items_.push_back(Item(std::move(text)));
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

class ListDescr: public List {
 public:
  explicit ListDescr(std::vector<ItemDescr>&& items)
      : items_(std::move(items))
      , max_str_len_(0) {
    for (const auto& e: items) {
      size_t len = e.LabelLength();
      if (len > max_str_len_) {
        max_str_len_ = len;
      }
    }
  }

  void Print(size_t i, size_t num_cols) const override {
    items_[i].Print(num_cols, max_str_len_);
  }

  const std::string& Value(size_t i) const override {
    return items_[i].Value();
  }

  const std::string& StrWithStyle(size_t i) const override {
    return items_[i].LabelWithStyle();
  }

  size_t Size() const override {
    return items_.size();
  }

 private:
  std::vector<ItemDescr> items_;
  size_t max_str_len_;
};

}  // namespace readline

#endif  // READLINE_TEXT_H
