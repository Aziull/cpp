#include <Parser/Parser.h>
#include <iostream>
#include <string>

void throwIfError(const ErrorType type, const std::string expected = "", int lineNum = 0, std::string lexer = "", std::string token = "")
{
    switch (type)
    {
    case ErrorType::UnknownInstruction:
    {
        throw std::runtime_error("Невідповідність інструкцій: очікувано " + expected +
                                 ", але отримано " + lexer + " " + token + "у рядку " +
                                 std::to_string(lineNum));
    }
    case ErrorType::UnpredictibleFileFinish:
    {
        throw std::runtime_error("Неочікуваний кінець програми: " + lexer + " " + token + " у рядку " +
                                 std::to_string(lineNum));
    }
    case ErrorType::UndefinedVariable:
    {
        throw std::runtime_error("Невизначений ідентифікатор " + lexer);
    }
    case ErrorType::UninitializedVariable:
    {
        throw std::runtime_error("Ідентифікатор не ініціалізовано " + lexer);
    }
    }
}

void Parser::parse()
{
    try
    {
        std::cout << "Entries in symbols table: " << std::to_string(_tableOfSymbols.size()) << std::endl;
        parseStatementList("");
        std::cout << "Syntax analysis complete successfully" << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cout << "Синтаксичний аналіз завершився з помилкою: \n"
                  << ex.what() << std::endl;
    }
}

bool Parser::parseStatementList(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "Parse statement list: " << std::endl;
    while (_rowNumber < _tableOfSymbols.size())
    {
        if (!parseStatement(logMessageAlignment + "\t"))
        {
            break;
        }
    }
    return true;
}

bool Parser::parseStatement(const std::string &logMessageAlignment)
{
    // Statement = VarDeclaration | Assignment | PrintStatement | IfStatement | ForStatement .
    std::cout << logMessageAlignment
              << "parse statement " << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    std::cout << "\t\t" << std::to_string(lineNumber) << " " << lexeme << " " << token << std::endl;
    if (token == "keyword")
    {
        return parseKeyword(logMessageAlignment + "\t");
    }
    else if (token == "ident")
    {
        return parseIdent(logMessageAlignment + "\t");
    }
    else
    {
        throwIfError(ErrorType::UnknownInstruction, "Keyword or ident", lineNumber, lexeme, token);
    }

    return false;
}

bool Parser::parseIdent(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment
              << "parse ident" << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "ident")
    {
        ++_rowNumber;
        return parseAssign(logMessageAlignment + "\t");
    }
    return false;
}

bool Parser::parseToken(const Lexeme &lexeme, const Token &token, const std::string &logMessageAlignment)
{
    const auto &[lineNumber, lex, tok, _] = _tableOfSymbols.at(_rowNumber);
    ++_rowNumber;
    if (_rowNumber >= _tableOfSymbols.size())
    {
        throwIfError(ErrorType::UnpredictibleFileFinish, "Unpredictible end of file at line", lineNumber);
    }
    std::cout << logMessageAlignment
              << "paser token " << lexeme << " " << token << std::endl;
    if (lexeme == lex && token == tok)
    {
        std::cout << logMessageAlignment
                  << "В рядку " << std::to_string(lineNumber) << " символ " << lex << " " << tok << std::endl;
        return true;
    }
    throwIfError(ErrorType::UnknownInstruction, "Expexted token " + token + " and lexeme " + lexeme, lineNumber, lex, tok);
    return true;
}

bool Parser::parseKeyword(const std::string &logMessageAlignment)
{
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "keyword" && lexeme == "def")
    {
        return parseDef(logMessageAlignment);
    }
    else if (token == "keyword" && lexeme == "if")
    {
        return parseIf(logMessageAlignment);
    }
    else if (token == "keyword" && lexeme == "for")
    {
        return parseFor(logMessageAlignment);
    }
    else
    {
        throwIfError(ErrorType::UnknownInstruction, "Expected keyword def, if or for", lineNumber, lexeme, token);
    }
    return false;
}

bool Parser::parseIf(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment
              << "parse if" << std::endl;
    parseToken("if", "keyword", logMessageAlignment + "\t");
    parseToken("(", "par_op", logMessageAlignment + "\t");
    parseBoolExpression(logMessageAlignment + "\t");
    parseToken(")", "par_op", logMessageAlignment + "\t");
    parseToken("{", "cur_par_op", logMessageAlignment + "\t");
    parseStatementList(logMessageAlignment + "\t");
    parseToken("}", "cur_par_op", logMessageAlignment + "\t");
    parseElse(logMessageAlignment + "\t");
    return true;
}

bool Parser::parseElse(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment
              << "parse else" << std::endl;
    if (_rowNumber >= _tableOfSymbols.size())
    {
        return true;
    }

    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "keyword" && lexeme == "else")
    {
        parseToken("else", "keyword", logMessageAlignment + "\t");
        parseToken("{", "cur_par_op", logMessageAlignment + "\t");
        parseStatementList(logMessageAlignment + "\t");
        parseToken("}", "cur_par_op", logMessageAlignment + "\t");
        return true;
    }
    return false;
}

bool Parser::parseFor(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment
              << "parse for" << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (lexeme == "for" && token == "keyword")
    {
        parseToken("(", "par_op", logMessageAlignment + "\t");
        const auto &[newLineNumber, newLexeme, newToken, newId] = _tableOfSymbols.at(_rowNumber);
        if (newToken == "ident")
        {
            parseIdent(logMessageAlignment + "\t");
        }
        else if (newToken == "def")
        {
            parseDef(logMessageAlignment + "\t");
        }
        parseToken(";", "split_op", logMessageAlignment + "\t");
        {
            const auto &[nl, l, t, _] = _tableOfSymbols.at(_rowNumber);
            if (t != "split_op")
            {
                parseBoolExpression(logMessageAlignment + "\t");
            }
        }

        parseToken(";", "split_op", logMessageAlignment + "\t");
        {
            const auto &[nl, l, t, _] = _tableOfSymbols.at(_rowNumber);
            if (t == "ident")
            {
                parseIdent(logMessageAlignment + "\t");
            }
        }
        parseToken(")", "par_op", logMessageAlignment + "\t");
        parseToken("{", "cur_par_op", logMessageAlignment + "\t");
        parseStatementList(logMessageAlignment + "\t");
        parseToken("}", "cur_par_op", logMessageAlignment + "\t");
    }
    return false;
}

bool Parser::parseDef(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << " parse def" << std::endl;
    parseToken("def", "keyword", logMessageAlignment + "\t");
    const auto &[lineNumber, lexeme, token, _] = _tableOfSymbols.at(_rowNumber);
    if (token == "ident")
    {
        ++_rowNumber;
        if (_rowNumber >= _tableOfSymbols.size())
        {
            return false;
        }
        const auto &[newLineNumber, newLexeme, assignToken, _] = _tableOfSymbols.at(_rowNumber);
        if (assignToken == "assign_op")
        {
            parseAssign(logMessageAlignment + "\t");
        }
    }
    return true;
}
bool Parser::parseBoolExpression(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << " parse bool expression" << std::endl;
    parseExpression(logMessageAlignment + "\t");
    const auto &[lineNumber, lexeme, token, _] = _tableOfSymbols.at(_rowNumber);
    if (token == "not_op" || token == "less_op" || token == "more_op" || token == "not_eql_op" || token == "less_than_op" || token == "eql_op" || token == "more_than_op")
    {
        ++_rowNumber;
    }
    else
    {
        // error
    }
    parseExpression(logMessageAlignment + "\t");
    return true;
}

bool Parser::parseAssign(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "parse assign" << std::endl;
    const auto &[lineNumber, lexeme, token, _] = _tableOfSymbols.at(_rowNumber);
    if (parseToken("=", "assign_op", logMessageAlignment + "\t"))
    {
        parseExpression(logMessageAlignment + "\t");
    }
    return false;
}
bool Parser::parseExpression(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "Parse expression " << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    std::cout << logMessageAlignment << std::to_string(lineNumber) << " " << lexeme << " " << token << " " << std::to_string(id) << std::endl;
    parseTerm(logMessageAlignment + "\t");
    while (_rowNumber < _tableOfSymbols.size())
    {
        const auto &[ln, l, t, i] = _tableOfSymbols.at(_rowNumber);
        if (t == "add_op")
        {
            ++_rowNumber;
            parseTerm(logMessageAlignment + "\t");
        }
        else
        {
            break;
        }
    }

    return true;
}
bool Parser::parseTerm(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "parse term " << std::endl;
    parseFactor(logMessageAlignment + "\t");
    // continue factor parsing
    while (_rowNumber < _tableOfSymbols.size())
    {
        const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
        if (token == "mult_opt")
        {
            ++_rowNumber;
            std::cout << logMessageAlignment << "в рядку " << lineNumber << " - " << lexeme << " " << token << std::endl;
            parseFactor(logMessageAlignment + "\t");
        }
        else
        {
            break;
        }
    }
    return true;
}

bool Parser::parseFactor(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "parse factor " << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "int" || token == "float" || token == "ident")
    {
        ++_rowNumber;
        std::cout << logMessageAlignment << "в рядку " << lineNumber << " - " << lexeme << " " << token << std::endl;
    }
    else if (lexeme == "(")
    {
        ++_rowNumber;
        parseExpression(logMessageAlignment + "\t");
        parseToken(")", "par_op", logMessageAlignment + "\t");
    }
    else
    {
        throwIfError(ErrorType::UnpredictibleFileFinish, "Factor parsing error", lineNumber, lexeme, token);
    }
    return true;
}
