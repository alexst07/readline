#include <iostream>

#include "src/readline.h"

int main() {
  using namespace readline;

  Readline readline;
  std::string line = readline.Prompt(">> ");
  std::cout << "string: " << line << std::endl;
  return 0;
}
