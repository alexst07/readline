#ifndef READLINE_UTILS_H
#define READLINE_UTILS_H

#include <memory>
#include <vector>
#include <string>

namespace readline {

std::vector<std::string> SplitArgs(const std::string& line, int line_pos);

bool CheckNewArg(const std::string& line, int line_pos);

enum class ListDirType {
  FILES,
  DIR,
  FILES_DIR
};

std::vector<std::string> ListDir(const std::string& dir, ListDirType t);

std::vector<std::string> MatchArg(const std::string& arg,
    std::vector<std::string> list);
}

#endif  // READLINE_UTILS_H
