#include <iostream>

#include "src/readline.h"
#include "src/utils.h"
#include "src/log.h"

int main() {
  using namespace readline;

  Log::Instance("log.txt");

  LOG << "TESTE_OTDER\n";

  std::function<std::tuple<std::unique_ptr<List>, RetType, bool>(
      const std::vector<std::string>&, bool)> fn =
          [](const std::vector<std::string>& params, bool tip) {
    std::vector<std::string> list;
    bool is_path = false;
    RetType ret_type;

    if (params.size() < 3) {
      list = {"test1", "outro", "qualquer", "test2"};

      if (tip) {
        if (!params.back().empty()) {
          // uses only item that match
          list = MatchArg(params.back(), list);
        }
      }
      ret_type = RetType::LIST;
    } else {
      ret_type = RetType::FILES_DIR;
      std::string last_arg = params.back();

      std::string path;
      std::string last_part;
      std::tie(path, last_part) = ParserPath(last_arg);

      if (IsDirectory(path)) {
        is_path = true;
        if (tip) {
          auto items = ListDir(path, ListDirType::FILES_DIR);

          if (!last_part.empty()) {
            // uses only item that match
            items = MatchArg(last_part, items);
          }

          return std::tuple<std::unique_ptr<List>, RetType, bool>(
              std::unique_ptr<List>(new ListItem(items)), ret_type, is_path);
        } else {
          return std::tuple<std::unique_ptr<List>, RetType, bool>(
              MatchDirList(params), ret_type, is_path);
        }
      }
    }

    return std::tuple<std::unique_ptr<List>, RetType, bool>(
        std::unique_ptr<List>(new ListItem(list)), ret_type, is_path);
  };

  std::function<Text(const std::string&)> fn_color = [](
      const std::string& str) {
    Text text;
    text << Style("\e[34m");

    for (int i = 0; i < str.length(); i++) {
      if (str[i] == ' ') {
        text << Style("\e[0m");
      }

      char c = str[i];
      std::string s;
      s += c;
      text << s;
    }

    return text;
  };

  Readline readline;
  readline.SetCompleteFunc(std::move(fn));
  readline.SetHighlightFunc(std::move(fn_color));
  readline.AddHistoryString("git commit -m \"assdf\"");
  readline.AddHistoryString("git add");
  readline.AddHistoryString("git rm");
  readline.AddHistoryString("ls");
  readline.AddHistoryString("cd home");
  Text prompt;
  prompt << Style("\e[34m");
  prompt << ">> ";
  prompt << Style("\e[0m");
  std::string line = readline.Prompt(prompt);
  std::cout << "string: " << line << std::endl;
  return 0;
}
