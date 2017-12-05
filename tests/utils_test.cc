#include <iostream>

#include "src/utils.h"

int main() {
  using namespace readline;
  std::string last, path;
  std::tie(path, last) = ParserPath("/teste/other/file");
  std::cout << path << std::endl << last << std::endl;

  std::tie(path, last) = ParserPath("///");
  std::cout << path << std::endl << last << std::endl;

  std::tie(path, last) = ParserPath("sdf");
  std::cout << path << std::endl << last << std::endl;

  std::tie(path, last) = ParserPath("/home/alex/");
  std::cout << path << std::endl << last << std::endl;

  std::tie(path, last) = ParserPath("");
  std::cout << path << std::endl << last << std::endl;

  std::tie(path, last) = ParserPath("~/Projects");
  std::cout << path << std::endl << last << std::endl;
}
