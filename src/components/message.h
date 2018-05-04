#ifndef READLINE_MESSAGE_H
#define READLINE_MESSAGE_H

#include <unordered_map>
#include <functional>
#include <iostream>

namespace readline {

class Message {
 public:
  enum class Type {
    ENTER,
    RIGHT_ARROW,
    LEFT_ARROW,
    UP_ARROW,
    DOWN_ARROW,
    DEL,
    ESQ,
    TAB,
    CHAR
  };

  Message(Type type): type_(type), has_payload_(false) {}

  Message(Type type, wchar_t payload)
      : type_(type)
      , payload_(payload)
      , has_payload_(true) {}

  Type type() const {
    return type_;
  }

  bool has_payload() const {
    return has_payload_;
  }

  wchar_t payload() const {
    return payload_;
  }

 private:
  Type type_;
  wchar_t payload_;
  bool has_payload_;
};

}  // namespace readline

#endif  // READLINE_MESSAGE_H
