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
    LOG << "Params size: " << params.size() << "\n";
    std::vector<std::string> list;
    bool is_path = false;
    RetType ret_type;

    if (params.size() <= 1) {
      list = {"test1", "outro", "qualquer", "test2"};

      if (tip) {
        if (!params.back().empty()) {
          // uses only item that match
          list = MatchArg(params.back(), list);
        }
      }
      ret_type = RetType::LIST;
    } else if (params.size() == 2) {
      std::vector<ItemDescr> list_descr = {
        ItemDescr("val1", Text("val1sdfasd"), Text("a short description for test maybe asdf asdf asdf ")),
        ItemDescr("val2", Text("val2asdfasdfaers"), Text("a short description2asdf a asdf asdf ewfas afsd asdf asdf asdfasdf asdf asdf asdf asdf asdf asd fasdf asdf asdf asdf aera")),
        ItemDescr("nice1", Text("nice1"), Text("a nice description")),
        ItemDescr("nice2", Text("nice2"), Text("a nice description")),
        ItemDescr("nice3", Text("nice3"), Text("a nice description")),
        ItemDescr("nice4", Text("nice4"), Text("a nice description")),
        ItemDescr("nice5", Text("nice5"), Text("a nice description")),
        ItemDescr("nice6", Text("nice6"), Text("a nice description")),
        ItemDescr("nice7", Text("nice7"), Text("a nice description")),
        ItemDescr("nice8", Text("nice8"), Text("a nice description")),
        ItemDescr("nice9", Text("nice9"), Text("a nice description")),
        ItemDescr("nice10", Text("nice10"), Text("a nice description")),
        ItemDescr("nice11", Text("nice11"), Text("a nice description")),
        ItemDescr("nice12", Text("nice12"), Text("a nice description")),
        ItemDescr("nice13", Text("nice13"), Text("a nice description")),
        ItemDescr("nice14", Text("nice14"), Text("a nice description")),
        ItemDescr("nice15", Text("nice15"), Text("a nice description")),
        ItemDescr("nice16", Text("nice16"), Text("a nice description")),
        ItemDescr("nice17", Text("nice17"), Text("a nice description")),
        ItemDescr("nice18", Text("nice18"), Text("a nice description")),
        ItemDescr("nice19", Text("nice19"), Text("a nice description")),
        ItemDescr("nice20", Text("nice20"), Text("a nice description")),
        ItemDescr("nice21", Text("nice21"), Text("a nice description")),
        ItemDescr("nice22", Text("nice22"), Text("a nice description")),
        ItemDescr("nice10", Text("nice10"), Text("a nice description")),
        ItemDescr("nice11", Text("nice11"), Text("a nice description")),
        ItemDescr("nice12", Text("nice12"), Text("a nice description")),
        ItemDescr("nice13", Text("nice13"), Text("a nice description")),
        ItemDescr("nice14", Text("nice14"), Text("a nice description")),
        ItemDescr("nice15", Text("nice15"), Text("a nice description")),
        ItemDescr("nice16", Text("nice16"), Text("a nice description")),
        ItemDescr("nice17", Text("nice17"), Text("a nice description")),
        ItemDescr("nice18", Text("nice18"), Text("a nice description")),
        ItemDescr("nice19", Text("nice19"), Text("a nice description")),
        ItemDescr("nice20", Text("nice20"), Text("a nice description")),
        ItemDescr("nice21", Text("nice21"), Text("a nice description"))
      };

      if (tip) {
        if (!params.back().empty()) {
          // uses only item that match
          list_descr = MatchArg(params.back(), list_descr);
        }
      }

      return std::tuple<std::unique_ptr<List>, RetType, bool>(
          std::unique_ptr<List>(new ListDescr(std::move(list_descr))), ret_type, is_path);
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
