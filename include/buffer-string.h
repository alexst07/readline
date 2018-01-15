#ifndef READLINE_BUFFER_STRING_H
#define READLINE_BUFFER_STRING_H

#include <string>

#include "utils.h"

namespace readline {

class BufferString {
 public:
  BufferString() = default;

  BufferString(const std::wstring& str): content_(str) {}

  BufferString(const BufferString& bufstr): content_(bufstr.content_) {}

  BufferString(BufferString&& bufstr): content_(std::move(bufstr.content_)) {}

  BufferString& operator=(const std::wstring& content);

  BufferString& operator=(const std::string& content);

  void PopBack();

  void RemoveChar(int n);

  void AddChar(wchar_t c, int n);

  void AddString(const std::wstring str, int n);

  void ReplaceStringInterval(const std::wstring str, int start, int end);

  bool IsTokenSeparator(int pos);

  int StartTokenPos(int n);

  int EndTokenPos(int n);

  void RemoveSubStr(int from, int to);

  bool IsEscapeSpace(int n);

  bool IsLastToken(int n);

  // arg is any sequence of chars seperated by spaces
  int StartArgPos(int n);

  int EndArgPos(int n);

  std::wstring GetTrimToken(int n);

  std::wstring GetSlice(int start, int end);

  const std::wstring& WStr() const;

  std::string Str() const {
    return wstr2str(content_);
  }

  BufferString& operator+=(wchar_t c);

  wchar_t& operator[](int n) {
    return content_[n];
  }

  inline int Length() const {
    return content_.length();
  }

 private:
  friend BufferString operator+(const BufferString& bufstr, wchar_t c);

  friend std::wostream& operator<<(std::wostream& stream, BufferString& buf_str);

  std::wstring content_;
};

BufferString operator+(const BufferString& bufstr, wchar_t c);

std::wostream& operator<<(std::wostream& stream, BufferString& buf_str);

}  // namespace readline

#endif  // READLINE_BUFFER_STRING_H
