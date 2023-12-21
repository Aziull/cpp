#pragma once
#include <Common/Types.h>

class Parser
{
public:
    Parser(TableOfSymbols tableOfSymbols) : _tableOfSymbols{std::move(tableOfSymbols)} {};

    void parse();

private:
    bool parseStatementList();
    bool parseStatement();
    bool parseIdent();
    bool parseToken(const Lexeme &lexeme, const Token &token);
    bool parseKeyword();
    bool parseIf();
    bool parseElse();
    bool parseFor();
    bool parseDef();
    bool parseBoolExpression();
    bool parseAssign();
    bool parseExpression();
    bool parseTerm();
    bool parseFactor();

private:
    TableOfSymbols _tableOfSymbols;
    std::size_t _rowNumber = 0;
    std::string _error = "";
};