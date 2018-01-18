#include <iostream>

#include "readline.h"

int main() {
  using namespace readline;

  LOG << "TESTE_OTDER\n";

  std::function<std::tuple<std::unique_ptr<List>, RetType, bool>(
      const std::vector<std::string>&, bool)> fn =
          [](const std::vector<std::string>& params, bool tip) {
    LOG << "Params size: " << params.size() << "\n";

    if (params.size() <= 1) {
      std::vector<std::string> list = {"test1", "outro", "qualquer", "test2"};
      return RetList(std::move(list), params, tip);
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

      return RetList(std::move(list_descr), params, tip);
    } else {
      return RetDirFileList(params, tip, ListDirType::FILES_DIR);
    }
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

  while (true) {
    boost::optional<std::string> line = readline.Prompt(prompt);

    if (line) {
      std::cout << "string: " << *line << std::endl;
    } else {
      std::cout << "exited\n";
      break;
    }
  }

  return 0;
}
