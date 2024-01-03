#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <stack>
#include <variant>
#include <tuple>
#include <utility>

enum Section
{
    Variables,
    Labels,
    Constants,
    Code
};

class PSM
{
public:
    // PSM(/*table of lables*/ const std::unordered_map<std::string, int> &tableOfId, const std::vector<std::pair<std::string, std::string>> &postfixCode);
    PSM() = default;
    // load from file
    void loadPostfixFile(const std::string &fileName);
    void postfixExec();
    void printState() const;

private:
    std::vector<std::string> readPostfixFileContent(const std::string &fileName);
    void parsePostfixFileContent(std::vector<std::string> postfixFileContent);
    std::size_t parseHeader(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex, const std::string &header);
    std::size_t parseLine(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex, const std::string &header);
    std::size_t parseCodeItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex);
    std::size_t parseVariableItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex);
    std::size_t parseLableItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex);
    std::size_t parseConstansItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex);
    std::size_t parseSectionItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex, Section section);
    std::size_t parseSection(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex, Section section);
    void printStack() const;
    void doJump(const std::string &lex, const std::string &tok);
    void doIt(const std::string &lex, const std::string &tok);
    void doAssign();
    void doNeg();
    void doArithmetic(const std::string &lex, const std::string &tok);
    void doBool(const std::string &lex, const std::string &tok);
    void doNot();
    std::pair<std::variant<int, float>, std::string> getValueAndType(const std::string &lex, const std::string &tok);

    std::vector<std::pair<std::string, std::string>> _postfixCode;
    using Value = std::variant<std::monostate, int, float>;
    struct Data
    {
        std::size_t _index;
        std::string _type;
        Value _value;
    };
    std::unordered_map<std::string, Data> _tableOfId;
    std::unordered_map<std::string, Data> _tableOfConstants;
    std::unordered_map<std::string, std::string> _tableOfLabels;
    std::size_t _instructionsCount;
    std::size_t _currentInstructionIndex;
    std::stack<std::pair<std::string, std::string>> _stack;
};