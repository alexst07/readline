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

  const std::string& Str() const;

  friend BufferString operator+(const BufferString& bufstr, char c);

  friend std::ostream& operator<<(std::ostream& stream, BufferString& buf_str);

  BufferString& operator+=(char c);

  void IncrementNewLine() {
    content_ += "\n";
  }

  void DecrementNewLine() {
    content_.pop_back();
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
