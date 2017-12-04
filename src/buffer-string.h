#ifndef READLINE_BUFFER_STRING_H
#define READLINE_BUFFER_STRING_H

#include <string>

namespace readline {

class BufferString {
 public:
  BufferString() = default;

  BufferString(const std::string& str): content_(str) {}

  BufferString(const BufferString& bufstr): content_(bufstr.content_) {}

  BufferString(BufferString&& bufstr): content_(std::move(bufstr.content_)) {}

  void PopBack();

  void RemoveChar(int n);

  void AddChar(char c, int n);

  bool IsTokenSeparator(int pos);

  int StartTokenPos(int n);

  int EndTokenPos(int n);

  void RemoveSubStr(int from, int to);

  bool IsEscapeSpace(int n);

  bool IsLastToken(int n);

  // arg is any sequence of chars seperated by spaces
  int StartArgPos(int n);

  int EndArgPos(int n);

  std::string GetTrimToken(int n);

  const std::string& Str() const;

  friend BufferString operator+(const BufferString& bufstr, char c);

  friend std::ostream& operator<<(std::ostream& stream, BufferString& buf_str);

  BufferString& operator+=(char c);

  char& operator[](int n) {
    return content_[n];
  }

  inline int Length() const {
    return content_.length();
  }

 private:
  std::string content_;
};

BufferString operator+(const BufferString& bufstr, char c);

std::ostream& operator<<(std::ostream& stream, BufferString& buf_str);

}  // namespace readline

#endif  // READLINE_BUFFER_STRING_H
