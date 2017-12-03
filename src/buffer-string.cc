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

bool BufferString::IsTokenSeparator(int pos) {
  if (pos >= static_cast<int>(content_.length()) || pos < 0) {
    return true;
  }

  char c = content_[pos];
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9')) {
    return false;
  }

  if (pos > 1 && pos < static_cast<int>(content_.length() - 1)) {
    if ((c == '\\' && content_[pos + 1] == ' ') ||
        (c == ' ' && content_[pos - 1] == '\\')) {
      return false;
    }
  }

  return true;
}

int BufferString::EndTokenPos(int n) {
  bool start_space = true;
  int pos = n;

  while ((start_space || !IsTokenSeparator(pos)) &&
      (pos <= static_cast<int>(content_.length()))) {
    if (content_[pos] != ' ') {
      start_space = false;
    }

    ++pos;
  }

  if (pos >= static_cast<int>(content_.length())) {
    return content_.length();
  }

  return pos;
}

int BufferString::StartTokenPos(int n) {
  if (n <= 1) {
    return 0;
  }

  bool start_space = true;
  int pos = n;

  while ((start_space || !IsTokenSeparator(pos - 1)) && (pos > 0)) {
    if (content_[pos - 1] != ' ') {
      start_space = false;
    }

    --pos;
  }

  return pos;
}

bool BufferString::IsLastToken(int n) {
  int pos = EndTokenPos(n);

  if (pos == content_.length()) {
    return true;
  }

  return false;
}

bool BufferString::IsEscapeSpace(int n) {
  if (content_.length() <= 1) {
    return false;
  }

  if (n > static_cast<int>(content_.length() - 1)) {
    n = content_.length() - 1;
  }

  if (content_[n] == ' ' && content_[n] == '\\') {
    return true;
  }

  return false;
}

void BufferString::RemoveSubStr(int from, int to) {
  content_ = content_.erase(from, to - from);
}

std::ostream& operator<<(std::ostream& stream, BufferString& buf_str) {
  stream  << buf_str.content_;

  return stream;
}

}  // namespace readline
