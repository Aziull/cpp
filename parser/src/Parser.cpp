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
                                 ", але отримано " + lexer + " " + token + " у рядку " +
                                 std::to_string(lineNum));
    }
    case ErrorType::UnpredictibleFileFinish:
    {
        throw std::runtime_error("Неочікуваний кінець програми: " + expected);
    }
    case ErrorType::UndefinedVariable:
    {
        throw std::runtime_error("Невизначений ідентифікатор " + lexer);
    }
    case ErrorType::UninitializedVariable:
    {
        throw std::runtime_error("Ідентифікатор не ініціалізовано " + lexer);
    }
    case ErrorType::IdentifierAlreadyDefined:
    {
        throw std::runtime_error("Ідентифікатор вже визначено " + lexer);
    }
    }
}

void Parser::printTableOfSymbols() const
{
    std::cout << std::string(30, '-') << std::endl;
    std::cout << "tableOfSymb:" << std::endl;
    for (const auto &item : _tableOfSymbols)
    {
        std::cout << "Key:\t" << item.first << "\tValue:("
                  << get<0>(item.second) << ", " << get<1>(item.second) << ", "
                  << get<2>(item.second) << ", " << get<3>(item.second) << ")" << std::endl;
    }
}

void Parser::parse()
{
    try
    {
        std::cout << "Entries in symbols table: " << std::to_string(_tableOfSymbols.size()) << std::endl;
        parseStatementList("");
        std::cout << "Syntax analysis complete successfully" << std::endl;
        printTableOfSymbols();
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
    std::cout << logMessageAlignment
              << "parse statement " << std::endl;
    if (isNextToken("keyword"))
    {
        return parseKeyword(logMessageAlignment + "\t");
    }
    else if (isNextToken("ident"))
    {
        return parseIdent(logMessageAlignment + "\t");
    }
    else
    {
        std::cout << logMessageAlignment << "No statement because no keyword or ident" << std::endl;
        return false;
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
        if (_variables.count(lexeme) == 0)
        {
            throwIfError(ErrorType::UndefinedVariable, "", lineNumber, lexeme, token);
        }
        ++_rowNumber;
        return parseAssign(logMessageAlignment + "\t", lexeme);
    }
    return true;
}

bool Parser::isNextToken(const Token &token, const Lexeme &lexeme)
{
    const auto &[lineNumber, lex, tok, _] = _tableOfSymbols.at(_rowNumber);
    // std::cout << lex << " " << tok << std::endl;
    return (tok == token) && (lexeme.empty() ? true : lex == lexeme);
}

bool Parser::parseToken(const Lexeme &lexeme, const Token &token, const std::string &logMessageAlignment)
{
    if (_rowNumber >= _tableOfSymbols.size())
    {
        throwIfError(ErrorType::UnpredictibleFileFinish, "Очікувався символ " + lexeme + " " + token);
    }
    const auto &[lineNumber, lex, tok, _] = _tableOfSymbols.at(_rowNumber);
    ++_rowNumber;
    std::cout << logMessageAlignment
              << "parse token: очікується " << lexeme << " " << token << std::endl;
    if (lexeme == lex && token == tok)
    {
        std::cout << logMessageAlignment
                  << "рядок " << std::to_string(lineNumber) << ": знайдено символ " << lex << " " << tok << std::endl;
        return true;
    }
    throwIfError(ErrorType::UnknownInstruction, " token " + token + " and lexeme " + lexeme, lineNumber, lex, tok);
    return true;
}

bool Parser::parseKeyword(const std::string &logMessageAlignment)
{

    if (isNextToken("keyword", "def"))
    {
        return parseDef(logMessageAlignment);
    }
    else if (isNextToken("keyword", "if"))
    {
        return parseIf(logMessageAlignment);
    }
    else if (isNextToken("keyword", "for"))
    {
        return parseFor(logMessageAlignment);
    }
    else if (isNextToken("keyword", "println"))
    {
        return parsePrintln(logMessageAlignment);
    }
    else if (isNextToken("keyword", "read"))
    {
        return parseRead(logMessageAlignment);
    }
    else
    {
        const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
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
    if (isNextToken("cur_par_op", "}"))
    {
        std::cout << logMessageAlignment << "\tstatement list empty in if block" << std::endl;
    }
    else
    {
        parseStatementList(logMessageAlignment + "\t");
    }
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
        std::cout << logMessageAlignment << "Блок else відсутній. Кінець файлу" << std::endl;
        return true;
    }

    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "keyword" && lexeme == "else")
    {
        parseToken("else", "keyword", logMessageAlignment + "\t");
        parseToken("{", "cur_par_op", logMessageAlignment + "\t");
        if (isNextToken("cur_par_op", "}"))
        {
            std::cout << logMessageAlignment << "\tstatement list empty in else block" << std::endl;
        }
        else
        {
            parseStatementList(logMessageAlignment + "\t");
        }
        parseToken("}", "cur_par_op", logMessageAlignment + "\t");
        return true;
    }
    else
    {
        std::cout << logMessageAlignment << "no else block" << std::endl;
        return true;
    }
    return false;
}

bool Parser::parseFor(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment
              << "parse for" << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    parseToken("for", "keyword", logMessageAlignment + "\t");
    parseToken("(", "par_op", logMessageAlignment + "\t");
    if (isNextToken("ident"))
    {
        parseIdent(logMessageAlignment + "\t");
    }
    else if (isNextToken("keyword", "def"))
    {
        parseDef(logMessageAlignment + "\t");
    }
    parseToken(";", "split_op", logMessageAlignment + "\t");
    if (!isNextToken("split_op"))
    {
        parseBoolExpression(logMessageAlignment + "\t");
    }
    parseToken(";", "split_op", logMessageAlignment + "\t");
    if (isNextToken("ident"))
    {
        parseIdent(logMessageAlignment + "\t");
    }
    parseToken(")", "par_op", logMessageAlignment + "\t");
    parseToken("{", "cur_par_op", logMessageAlignment + "\t");
    if (isNextToken("cur_par_op", "}"))
    {
        std::cout << logMessageAlignment << "\tstatement list empty in for block" << std::endl;
    }
    else
    {
        parseStatementList(logMessageAlignment + "\t");
    }
    parseToken("}", "cur_par_op", logMessageAlignment + "\t");
    return true;
}

bool Parser::parseDef(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "parse def" << std::endl;
    parseToken("def", "keyword", logMessageAlignment + "\t");
    const auto &[lineNumber, lex, tok, _] = _tableOfSymbols.at(_rowNumber);
    if (isNextToken("ident"))
    {
        if (_variables.count(lex) == 0)
        {
            _variables[lex] = false;
            std::cout << logMessageAlignment << "оголошено ідентифікатор " << lex << std::endl;
        }
        else
        {
            throwIfError(ErrorType::IdentifierAlreadyDefined, "", lineNumber, lex, tok);
        }
        return parseIdent(logMessageAlignment + "\t");
    }
    else
    {
        throwIfError(ErrorType::UnknownInstruction, "ідентифікатор", lineNumber, lex, tok);
    }
    return false;
}
bool Parser::parseBoolExpression(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "parse bool expression" << std::endl;
    parseExpression(logMessageAlignment + "\t");
    const auto &[lineNumber, lexeme, token, _] = _tableOfSymbols.at(_rowNumber);
    if (token == "not_op" || token == "less_op" || token == "more_op" || token == "not_eql_op" || token == "less_than_op" || token == "eql_op" || token == "more_than_op")
    {
        ++_rowNumber;
    }
    else
    {
        throwIfError(ErrorType::UnknownInstruction, "оператор порівняння", lineNumber, lexeme, token);
    }
    parseExpression(logMessageAlignment + "\t");
    postfixCodeGeneration(lexeme, token);
    return true;
}

bool Parser::parseAssign(const std::string &logMessageAlignment, const std::string &identifier)
{
    std::cout << logMessageAlignment << "parse assign" << std::endl;
    const auto &[lineNumber, lexeme, token, _] = _tableOfSymbols.at(_rowNumber - 1);
    if (isNextToken("assign_op", "="))
    {
        postfixCodeGeneration(lexeme, token, "lval");
        parseToken("=", "assign_op", logMessageAlignment + "\t");
        parseExpression(logMessageAlignment + "\t");
        _variables[identifier] = true;
        std::cout << logMessageAlignment << "ідентифікатор " << identifier << " ініціалізовано" << std::endl;
        postfixCodeGeneration("=", "assign_op");
        return true;
    }
    return true;
}
bool Parser::parseExpression(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "Parse expression " << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    std::cout << logMessageAlignment << "в рядку " << std::to_string(lineNumber) << ": " << lexeme << " " << token << " " << std::to_string(id) << std::endl;
    parseTerm(logMessageAlignment + "\t");
    while (_rowNumber < _tableOfSymbols.size())
    {
        const auto &[ln, l, t, i] = _tableOfSymbols.at(_rowNumber);
        if (t == "add_op")
        {
            ++_rowNumber;
            parseTerm(logMessageAlignment + "\t");
            postfixCodeGeneration(l, t);
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
            postfixCodeGeneration(lexeme, token);
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
    auto &[lN, lex, tok, _] = _tableOfSymbols.at(_rowNumber);
    bool isNeg = false;
    if (lex == "-")
    {
        ++_rowNumber;
        tok = "neg";
        isNeg = true;
        std::cout << logMessageAlignment << "в рядку " << lN << ": унарний мінус"
                  << " token " << tok << " lexeme " << lex << std::endl;
    }
    auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "ident")
    {
        if (_variables.count(lexeme) == 0)
        {
            throwIfError(ErrorType::UndefinedVariable, "", lineNumber, lexeme, token);
        }
        else if (!_variables[lexeme])
        {
            throwIfError(ErrorType::UninitializedVariable, "", lineNumber, lexeme, token);
        }
        postfixCodeGeneration(lexeme, token, "rval");
        ++_rowNumber;
        std::cout << logMessageAlignment << "в рядку " << lineNumber << ": " << lexeme << " " << token << std::endl;
    }
    else if (token == "int" || token == "float")
    {
        postfixCodeGeneration(lexeme, token, "const");
        ++_rowNumber;
        std::cout << logMessageAlignment << "в рядку " << lineNumber << ": " << lexeme << " " << token << std::endl;
    }
    else if (lexeme == "(")
    {
        parseToken("(", "par_op", logMessageAlignment + "\t");
        parseExpression(logMessageAlignment + "\t");
        parseToken(")", "par_op", logMessageAlignment + "\t");
    }
    else
    {
        throwIfError(ErrorType::UnknownInstruction, "вираз", lineNumber, lexeme, token);
    }
    if (isNeg)
    {
        postfixCodeGeneration("-", "neg");
    }
    return true;
}
bool Parser::parsePrintln(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "parse println " << std::endl;
    parseToken("println", "keyword", logMessageAlignment + "\t");
    parseToken("(", "par_op", logMessageAlignment + "\t");
    parseExpression(logMessageAlignment + "\t");
    parseToken(")", "par_op", logMessageAlignment + "\t");
    postfixCodeGeneration("println", "out");
    return true;
}

bool Parser::parseRead(const std::string &logMessageAlignment)
{
    std::cout << logMessageAlignment << "parse read " << std::endl;
    parseToken("read", "keyword", logMessageAlignment + "\t");
    parseToken("(", "par_op", logMessageAlignment + "\t");
    auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    postfixCodeGeneration(lexeme, token, "lval");
    parseIdent(logMessageAlignment + "\t");
    parseToken(")", "par_op", logMessageAlignment + "\t");
    postfixCodeGeneration("read", "in");
    return true;
}

std::vector<std::pair<std::string, std::string>> Parser::getPostfixCode() const
{
    return _postfixCode;
}

void Parser::postfixCodeGeneration(const std::string &lexeme, const std::string &token, const std::string &lexCase)
{
    if (lexCase == "lval")
    {
        _postfixCode.push_back({lexeme, "lval"});
    }
    else if (lexCase == "rval")
    {
        _postfixCode.push_back({lexeme, "lval"});
    }
    else
    {
        _postfixCode.push_back({lexeme, token});
    }
}
