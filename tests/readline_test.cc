#include <iostream>

#include "src/readline.h"

int main() {
  using namespace readline;

  std::function<CompleteList(const std::vector<std::string>&, bool)> fn =
  [](const std::vector<std::string>& params, bool new_param) -> CompleteList {
    std::vector<std::string> list = {"test1", "test2", "test3"};
    return CompleteResultList(std::move(list));
  };

  Readline readline;
  std::string line = readline.Prompt(">> ");
  std::cout << "string: " << line << std::endl;
  return 0;
}
