#include <Parser/Parser.h>
#include <iostream>

void Parser::parse()
{
    std::cout << "\n\n\nEntries in symbols table: " << std::to_string(_tableOfSymbols.size()) << std::endl;
    parseStatementList();
    std::cout << "\nSyntax analysis complete successfully" << std::endl;
}

bool Parser::parseStatementList()
{
    while (_rowNumber < _tableOfSymbols.size())
    {
        if (!parseStatement())
        {
            break;
        }
    }
    return true;
}

bool Parser::parseStatement()
{
    // Statement = VarDeclaration | Assignment | PrintStatement | IfStatement | ForStatement .
    std::cout << "Parcing row number: " << std::to_string(_rowNumber) << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    std::cout << std::to_string(lineNumber) << " " << lexeme << " " << token << std::endl;
    if (token == "keyword")
    {
        return parseKeyword();
    }
    else if (token == "ident")
    {
        parseIdent();
    }

    return false;
}

bool Parser::parseIdent()
{
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "ident")
    {
        ++_rowNumber;
        return parseAssign();
    }
    return false;
}

bool Parser::parseToken(const Lexeme &lexeme, const Token &token)
{
    const auto &[lineNumber, lex, tok, _] = _tableOfSymbols.at(_rowNumber);
    std::cout << "В рядку " << std::to_string(lineNumber) << " символ " << lex << " " << tok << std::endl;
    ++_rowNumber;
    if (lexeme == lex && token == tok)
    {
        return true;
    }
    _error = "error in line " + std::to_string(lineNumber);
    return false;
}

bool Parser::parseKeyword()
{
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "keyword" && lexeme == "def")
    {
        return parseDef();
    }
    else if (token == "keyword" && lexeme == "if")
    {
        return parseIf();
    }
    else if (token == "keyword" && lexeme == "for")
    {
        return parseFor();
    }
    return false;
}

bool Parser::parseIf()
{
    parseToken("if", "keyword");
    parseToken("(", "par_op");
    parseBoolExpression();
    parseToken(")", "par_op");
    parseToken("{", "cur_par_op");
    parseStatementList();
    parseToken("}", "cur_par_op");
    parseElse();
    return true;
}

bool Parser::parseElse()
{
    if (_rowNumber >= _tableOfSymbols.size())
    {
        return false;
    }

    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "keyword" && lexeme == "else")
    {
        parseToken("else", "keyword");
        parseToken("{", "cur_par_op");
        parseStatementList();
        parseToken("}", "cur_par_op");
        return true;
    }
    return false;
}

bool Parser::parseFor()
{
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (lexeme == "for" && token == "keyword")
    {
        parseToken("(", "par_op");
        const auto &[newLineNumber, newLexeme, newToken, newId] = _tableOfSymbols.at(_rowNumber);
        if (newToken == "ident")
        {
            parseIdent();
        }
        else if (newToken == "def")
        {
            parseDef();
        }
        parseToken(";", "split_op");
        {
            const auto &[nl, l, t, _] = _tableOfSymbols.at(_rowNumber);
            if (t != "split_op")
            {
                parseBoolExpression();
            }
        }

        parseToken(";", "split_op");
        {
            const auto &[nl, l, t, _] = _tableOfSymbols.at(_rowNumber);
            if (t == "ident")
            {
                parseIdent();
            }
        }
        parseToken(")", "par_op");
        parseToken("{", "cur_par_op");
        parseStatementList();
        parseToken("}", "cur_par_op");
    }
    return false;
}

bool Parser::parseDef()
{
    parseToken("def", "keyword");
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
            parseAssign();
        }
    }
    return true;
}
bool Parser::parseBoolExpression()
{
    parseExpression();
    const auto &[lineNumber, lexeme, token, _] = _tableOfSymbols.at(_rowNumber);
    if (token == "not_op" || token == "less_op" || token == "more_op" || token == "not_eql_op" || token == "less_than_op" || token == "eql_op" || token == "more_than_op")
    {
        ++_rowNumber;
    }
    else
    {
        // error
    }
    parseExpression();
    return true;
}

bool Parser::parseAssign()
{
    const auto &[lineNumber, lexeme, token, _] = _tableOfSymbols.at(_rowNumber);
    if (parseToken("=", "assign_op"))
    {
        parseExpression();
    }
    return false;
}
bool Parser::parseExpression()
{
    std::cout << "Parse expression " << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    std::cout << std::to_string(lineNumber) << " " << lexeme << " " << token << " " << std::to_string(id) << std::endl;
    parseTerm();
    while (_rowNumber < _tableOfSymbols.size())
    {
        const auto &[ln, l, t, i] = _tableOfSymbols.at(_rowNumber);
        if (t == "add_op")
        {
            ++_rowNumber;
            parseTerm();
        }
        else
        {
            break;
        }
    }

    return true;
}
bool Parser::parseTerm()
{
    std::cout << "parse term " << std::endl;
    parseFactor();
    // continue factor parsing
    while (_rowNumber < _tableOfSymbols.size())
    {
        const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
        if (token == "mult_opt")
        {
            ++_rowNumber;
            std::cout << "в рядку " << lineNumber << " - " << lexeme << " " << token << std::endl;
            parseFactor();
        }
        else

        {
            break;
        }
    }
    return true;
}

bool Parser::parseFactor()
{
    std::cout << "parse factor " << std::endl;
    const auto &[lineNumber, lexeme, token, id] = _tableOfSymbols.at(_rowNumber);
    if (token == "int" || token == "float" || token == "ident")
    {
        ++_rowNumber;
        std::cout << "в рядку " << lineNumber << " - " << lexeme << " " << token << std::endl;
    }
    else if (lexeme == "(")
    {
        ++_rowNumber;
        parseExpression();
        parseToken(")", "par_op");
    }
    else
    {
        // error
    }
    return true;
}
