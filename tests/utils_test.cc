#include <iostream>

#include "src/utils.h"

int main() {
  using namespace readline;
  std::string last, path;
  std::tie(path, last) = ParserPath("/teste/other/file");
  std::cout << path << std::endl << last;
}
