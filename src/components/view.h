#ifndef READLINE_VIEW_H
#define READLINE_VIEW_H

#include <unordered_map>
#include <functional>
#include <iostream>

#include "message.h"

namespace readline {

struct Position {
  int line;
  int col;
};

struct Size {
  int lines;
  int cols;
};

class View;

using Action = std::function<void(View*)>;

enum class Event {
  KEY_EVENT
};

class View {
 public:
  View(View* parent, int line, int col)
    : parent_(parent)
    , pos_{line, col}
    , focus_(false) {}

  View(View* parent, const Position& pos)
    : parent_(parent)
    , pos_(pos)
    , focus_(false) {}

  View(View* parent)
    : parent_(parent)
    , pos_{1, 1}
    , focus_(false) {}

  virtual void Draw() = 0;

  virtual void ProcessMessage(const Message& msg) = 0;

  void Render() {
    Draw();

    if (parent_) {
      parent_->Draw();
    }
  }

  const Position& position() const {
    return pos_;
  }

  void position(const Position& pos) {
    pos_.line = pos.line;
    pos_.col = pos.col;
  }

  void position(int line, int col) {
    pos_.line = line;
    pos_.col = col;
  }

  void RegisterEvent(Event event, Action&& action) {
    events_.insert(std::pair<Event, Action>(event, std::move(action)));
  }

  const std::unordered_map<Event, Action>& Events() const {
    return events_;
  }

  const bool focus() const {
    return focus_;
  }

  void focus(bool value) {
    focus_ = value;
    Render();
  }

  View* parent() {
    return parent_;
  }

  virtual Size size() const = 0;

 private:
  View* parent_;
  Position pos_;
  std::unordered_map<Event, Action> events_;
  bool focus_;
};

void CursorTo(int line, int col) {
  std::string escape = std::string("\033[") +
      std::to_string(line) + std::string(";") +
      std::to_string(col) + std::string("f");

  std::cout << escape;
}

void CursorTo(const Position& pos) {
  std::string escape = std::string("\033[") +
      std::to_string(pos.line) + std::string(";") +
      std::to_string(pos.col) + std::string("f");

  std::cout << escape;
}

}  // namespace readline

#endif  // READLINE_VIEW_H
