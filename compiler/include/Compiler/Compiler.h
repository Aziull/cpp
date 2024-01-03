#pragma once
#include <string>
#include <unordered_map>

class Compiler
{
public:
    Compiler() = default;
    void compileToPostfix(const std::string &fileName);

private:
    std::string readSourceCode(const std::string &fileName);
    void savePostfixCode(const std::string &fileName, const std::vector<std::pair<std::string, std::string>> &postfixCode, const std::unordered_map<std::string, std::pair<std::string, int>> &tableOfConstants, const std::unordered_map<std::string, int> &tableOfId, const std::unordered_map<std::string, std::string> &labels);
    void serv(const std::vector<std::pair<std::string, std::string>> &postfixCode, const std::unordered_map<std::string, int> &tableOfId);
};
