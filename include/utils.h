#ifndef READLINE_UTILS_H
#define READLINE_UTILS_H

#include <memory>
#include <vector>
#include <string>

#include "text.h"
#include "complete.h"

namespace readline {

std::vector<std::string> SplitArgs(const std::string& line, int line_pos);

bool CheckNewArg(const std::string& line, int line_pos);

enum class ListDirType {
  FILES,
  DIR,
  FILES_DIR
};

std::vector<std::string> ListDir(const std::string& dir, ListDirType t);

void MatchArg(const std::string& arg, List* list);

std::vector<std::string> MatchArg(const std::string& arg,
    std::vector<std::string>& list);

std::vector<ItemDescr> MatchArg(const std::string& arg,
    std::vector<ItemDescr>& list);

std::unique_ptr<List> MatchDirList(const std::vector<std::string>& args);

// parser the parser in last part and the rest, for example:
// /home/alex/test
// test is the last part and /home/alex/ is the rest
// supress point avoid that last part return as "." pwd
std::tuple<std::string, std::string> ParserPath(const std::string& arg,
    bool supress_point = true);

std::string DirectoryFormat(const std::string& dir);

bool IsDirectory(const std::string path);

std::tuple<std::unique_ptr<List>, RetType, bool> RetDirFileList(
    const std::vector<std::string>& params, bool tip, ListDirType type);

std::tuple<std::unique_ptr<List>, RetType, bool> RetList(
    std::vector<std::string>&& plist, const std::vector<std::string>& params,
    bool tip);

std::tuple<std::unique_ptr<List>, RetType, bool> RetList(
    std::vector<ItemDescr>&& plist_descr,
    const std::vector<std::string>& params, bool tip);

std::wstring str2wstr(const std::string& str);

std::string wstr2str(const std::wstring& wstr);

}

#endif  // READLINE_UTILS_H
