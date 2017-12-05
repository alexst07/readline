#include "buffer-string.h"

#include <string>
#include <boost/algorithm/string.hpp>

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

void BufferString::AddString(const std::string str, int n) {
  if (content_.empty()) {
    content_ = str;
    return;
  }

  if (n >= int(content_.length())) {
    content_ += str;
  } else if (n == 0) {
    content_ = str + content_;
  } else if (n > 0 && n < int(content_.length())) {
    content_ = content_.substr(0, n) + str + content_.substr(n);
  }
}

void BufferString::ReplaceStringInterval(const std::string str,
    int start, int end) {
  if (content_.empty()) {
    content_ = str;
    return;
  }

  if (end >= int(content_.length()) && start <= 0) {
    content_ = str;
  } else if (end >= int(content_.length())) {
    content_ = content_.substr(0, start) + str;
  } else if (start <= 0) {
    content_ = str + content_.substr(end);
  } else {
    content_ = content_.substr(0, start) + str + content_.substr(end);
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

int BufferString::StartArgPos(int n) {
  if (n <= 1) {
    return 0;
  }

  int pos = n;

  // to avoid problems in the case where you have spaces in front of the
  // argument, and start arg will get the next arg, and the arg we want
  // is the arg before
  if (pos > 0) {
    if (content_[pos] == ' ' && content_[pos - 1] != ' ') {
      --pos;
    } else if (content_[pos] == ' ' && content_[pos - 1] == ' ') {
      // if the current selected char is a whitespace, and the back char
      // is a whitespace too, return the current position
      return pos;
    }
  }

  while ((content_[pos] != ' ' ||IsEscapeSpace(pos)) && (pos > 0)) {
    --pos;
  }

  // when pos is not zero, is counts the whitespace
  // so it 'eat' one whitespace
  return (pos > 0? pos + 1:0);
}

int BufferString::EndArgPos(int n) {
  int pos = n;

  while ((content_[pos] != ' ' ||IsEscapeSpace(pos)) &&
      (pos <= static_cast<int>(content_.length()))) {
    ++pos;
  }

  if (pos >= static_cast<int>(content_.length())) {
    return content_.length();
  }

  return pos;
}

std::string BufferString::GetTrimToken(int n) {
  int start = StartArgPos(n);
  int end = EndArgPos(n);
  std::string str = content_.substr(start, end - start);
  boost::trim(str);
  return str;
}

std::string BufferString::GetSlice(int start, int end) {
  return content_.substr(start, end - start);
}

void BufferString::RemoveSubStr(int from, int to) {
  content_ = content_.erase(from, to - from);
}

std::ostream& operator<<(std::ostream& stream, BufferString& buf_str) {
  stream  << buf_str.content_;

  return stream;
}

}  // namespace readline
