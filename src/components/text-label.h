#ifndef READLINE_TEXT_LABEL_H
#define READLINE_TEXT_LABEL_H

#include <string>
#include <unordered_map>
#include <functional>

#include "view.h"

namespace readline {

class TextLabel: public View {
 public:
  TextLabel(View* parent, int line, int col): View(parent, line, col) {}
  TextLabel(View* parent, const Position& pos): View(parent, pos) {}
  TextLabel(View* parent): View(parent) {}

  void Draw() override {
    CursorTo(position());
    std::cout << fg_color_<< bg_color_<< text_ << "\e[0m" << std::endl;
  }

  void ProcessMessage(const Message& msg) override {}

  Size size() const override {
    Size s;
    s.lines = 1;
    s.cols = text_.length();

    return s;
  }

  void SetText(const std::string& text) {
    text_ = text;
    Render();
  }

  const std::string& GetText() const {
    return text_;
  }

  void SetForegroundColor(const std::string& color) {
    fg_color_ = color;
    Render();
  }

  void SetBackgroundColor(const std::string& color) {
    bg_color_ = color;
    Render();
  }

 private:
  std::string text_;
  std::string fg_color_;
  std::string bg_color_;
};

}  // namespace readline

#endif  // READLINE_TEXT_LABEL_H
