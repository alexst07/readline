#ifndef READLINE_READLINE_H
#define READLINE_READLINE_H

#include <string>

namespace readline {

class Readline {
 public:
  Readline() = default;

  std::string Prompt(const std::string& prompt);

 private:
  
};

}  // namespace readline

#endif  // READLINE_READLINE_H
