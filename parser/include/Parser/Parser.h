#pragma once
#include <Common/Types.h>

class Parser
{
public:
    Parser(TableOfSymbols tableOfSymbols) : _tableOfSymbols{std::move(tableOfSymbols)} {};

    void parse();

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
    bool parseAssign(const std::string &logMessageAlignment);
    bool parseExpression(const std::string &logMessageAlignment);
    bool parseTerm(const std::string &logMessageAlignment);
    bool parseFactor(const std::string &logMessageAlignment);

private:
    TableOfSymbols _tableOfSymbols;
    std::size_t _rowNumber = 0;
    std::string _error = "";
};