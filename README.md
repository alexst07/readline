# Readline

A modern readline implementation for GNU-Readline kind library.

Ps: This project is under development.

[![asciicast](https://asciinema.org/a/QJ9RnYlIZaJ4eJ7kHeWz2EDgH.png)](https://asciinema.org/a/QJ9RnYlIZaJ4eJ7kHeWz2EDgH)

## Example
```cpp
#include <iostream>
#include "readline.h"

int main() {
  // of course you must never use 'using namespace', but for example it
  // can be useful
  using namespace readline;

  // function to complete and tip arguments
  std::function<std::tuple<std::unique_ptr<List>, RetType, bool>(
      const std::vector<std::string>&, bool)> fn =
          [](const std::vector<std::string>& params, bool tip) {
    if (params.size() <= 1) {
      // on the first argument return this item list as result
      std::vector<std::string> list = {"test1", "other", "any", "test2"};
      return RetList(std::move(list), params, tip);
    } else if (params.size() == 2) {
      // on the first argument return this description list as result
      std::vector<ItemDescr> list_descr = {
        ItemDescr("item_descr1", Text("item_descr1"), Text("a short description for test")),
        ItemDescr("item_descr1", Text("item_descr1"), Text("a short description 2"))
      };

      return RetList(std::move(list_descr), params, tip);
    } else {
      // for others aruments return a file directory list
      return RetDirFileList(params, tip, ListDirType::FILES_DIR);
    }
  };

  // function to highlight the line
  std::function<Text(const std::string&)> fn_color = [](
      const std::string& str) {
    // format the first argument to set color white
    Text text;

    // add the style on the text
    // all style must be add with Style class
    text << Style("\e[34m");

    for (int i = 0; i < str.length(); i++) {
      if (str[i] == ' ') {
        text << Style("\e[0m");
      }

      char c = str[i];
      std::string s;
      s += c;

      // text have the operator << overloaded to receive data like std::cout
      text << s;
    }

    return text;
  };

  // instatiate Readline object
  Readline readline;

  // set the function to complete arguments
  readline.SetCompleteFunc(std::move(fn));

  // set function to highlight line
  readline.SetHighlightFunc(std::move(fn_color));

  // Add history commands
  readline.AddHistoryString("git commit -m \"assdf\"");
  readline.AddHistoryString("git add");
  readline.AddHistoryString("git rm");
  readline.AddHistoryString("ls");
  readline.AddHistoryString("cd home");

  // Set the prompt of the line
  Text prompt;
  prompt << Style("\e[34m");
  prompt << ">> ";
  prompt << Style("\e[0m");

  // This method in fact call the line
  boost::optional<std::string> line = readline.Prompt(prompt);

  if (line) {
    std::cout << "string: " << *line << std::endl;
  } else {
    std::cout << "exited\n";
  }

  return 0;
}
```

## Build
### Requirements:
* Boost

### Compile and Tests
#### To compile the project
On project directory:
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

#### To test the project
On build directory
```sh
./test/readline_test
```

#### To install readline
```sh
sudo make install
```

## How to contribute
This project is not ready for use yet, so any help is very welcome.
If you want help:
* Fork the project
* Create a branch in your fork for the feature or the correction
* Pull request on master branch

You can contact in the email: alexst07@gmail.com
