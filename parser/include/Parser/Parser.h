#pragma once
#include <Common/Types.h>
#include <vector>

class Parser
{
public:
    Parser(TableOfSymbols tableOfSymbols) : _tableOfSymbols{std::move(tableOfSymbols)} {};

    void parse();
    std::vector<std::pair<std::string, std::string>> getPostfixCode() const;
    std::vector<std::string> getIlCode() const;
    std::unordered_map<std::string, std::string> getLabels() const;

private:
    bool parseStatementList(const std::string &logMessageAlignment);
    bool parseStatement(const std::string &logMessageAlignment);
    bool parseIdent(const std::string &logMessageAlignment);
    bool parseToken(const Lexeme &lexeme, const Token &token, const std::string &logMessageAlignment);
    bool parseKeyword(const std::string &logMessageAlignment);
    bool parseIf(const std::string &logMessageAlignment);
    bool parseElse(const std::string &logMessageAlignment);
    bool parseFor(const std::string &logMessageAlignment);
    bool parseDef(const std::string &logMessageAlignment);
    bool parseBoolExpression(const std::string &logMessageAlignment);
    bool parseAssign(const std::string &logMessageAlignment, const std::string &identifier);
    bool parseExpression(const std::string &logMessageAlignment);
    bool parseTerm(const std::string &logMessageAlignment);
    bool parseFactor(const std::string &logMessageAlignment);
    bool parsePrintln(const std::string &logMessageAlignment);
    bool isNextToken(const Token &token, const Lexeme &lexeme = "");
    void printTableOfSymbols() const;
    bool parseRead(const std::string &logMessageAlignment);
    std::string createLabel();
    void setLabel(const std::string &label);

    void postfixCodeGeneration(const std::string &lexeme, const std::string &token, const std::string &lexCase = "");
    void ilCodeGeneration(const std::string &lexeme, const std::string &token, const std::string &lexCase = "");

private:
    TableOfSymbols _tableOfSymbols;
    std::unordered_map<std::string, bool> _variables;
    std::size_t _rowNumber = 0;
    std::string _error = "";
    std::vector<std::pair<std::string, std::string>> _postfixCode = {};
    std::unordered_map<std::string, std::string> _labels;
    std::vector<std::string> _ilCode = {};
};