#include "buffer-string.h"

#include <string>

namespace readline {

void BufferString::PopBack() {
  if (content_.length() > 0) {
    content_.pop_back();
  }
}

const std::string& BufferString::Str() const {
  return content_;
}

BufferString operator+(const BufferString& bufstr, char c) {
  std::string str = bufstr.content_ + c;

  return BufferString(str);
}

BufferString& BufferString::operator+=(char c) {
  content_ += c;

  return *this;
}

void BufferString::RemoveChar(int n) {
  if (n >= int(content_.length() - 1)) {
    content_.pop_back();
  } else if (n == 0) {
    content_ = content_.substr(1);
  } else if (n > 0 && n < int(content_.length())) {
    content_ = content_.substr(0, n) + content_.substr(n+1);
  }
}

void BufferString::AddChar(char c, int n) {
  if (content_.empty()) {
    content_ = c;
    return;
  }

  if (n >= int(content_.length())) {
    content_ += c;
  } else if (n == 0) {
    content_ = c + content_;
  } else if (n > 0 && n < int(content_.length())) {
    content_ = content_.substr(0, n) + c + content_.substr(n);
  }
}

std::ostream& operator<<(std::ostream& stream, BufferString& buf_str) {
  stream  << buf_str.content_;

  return stream;
}

}  // namespace readline
