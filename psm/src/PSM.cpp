#include <PSM/PSM.h>
#include <fstream>
#include <set>
#include <sstream>
#include <cmath>

namespace
{
    std::string trim(const std::string &str,
                     const std::string &whitespace = " \t")
    {
        const auto strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos)
            return ""; // no content

        const auto strEnd = str.find_last_not_of(whitespace);
        const auto strRange = strEnd - strBegin + 1;
        return str.substr(strBegin, strRange);
    }

    std::vector<std::string> splitStringToWords(const std::string &input)
    {
        // std::cout << "splitStringToWords \"" << input << "\"" << std::endl;
        std::istringstream iss(input);
        std::vector<std::string> words;
        std::string word;

        while (iss >> word)
        {
            words.push_back(word);
            // std::cout << std::to_string(words.size()) << "\t" << word << std::endl;
        }
        return words;
    }

    std::pair<std::variant<int, float>, std::string> parseNumber(const std::string &str)
    {
        try
        {
            size_t pos;
            if ((pos = str.find('.')) != std::string::npos)
            {
                // If the string contains a '.', parse as float
                size_t afterDecimal;
                float floatValue = std::stof(str, &afterDecimal);

                if (afterDecimal != str.length())
                {
                    throw std::invalid_argument("Invalid characters after decimal point");
                }

                return {floatValue, "float"};
            }
            else
            {
                // Otherwise, parse as integer
                size_t afterInteger;
                int intValue = std::stoi(str, &afterInteger);

                if (afterInteger != str.length())
                {
                    throw std::invalid_argument("Invalid characters after integer");
                }

                return {intValue, "int"};
            }
        }
        catch (const std::invalid_argument &e)
        {
            throw std::invalid_argument("Invalid input: " + str);
        }
    }

    std::unordered_map<Section, std::string> sectionHeaders{{Section::Variables, ".vars("}, {Section::Labels, ".labels("}, {Section::Constants, ".constants("}, {Section::Code, ".code("}};

    std::set<std::string> valueTokens = {"int", "float", "l-val", "r-val", "label"};
    std::set<std::string> jumpTokens = {"jump", "jf", "colon"};

    std::set<std::string> arithmeticOperatorTokens = {"add_op", "mult_op", "exp_op"};
    std::set<std::string> boolOperatorTokens = {"less_op", "more_op", "not_eql_op", "less_than_op", "eql_op", "more_than_op"};
}

std::vector<std::string> PSM::readPostfixFileContent(const std::string &fileName)
{
    std::ifstream f(fileName);
    if (f)
    {
        std::vector<std::string> content;
        std::string line;
        while (getline(f, line))
        {
            line = trim(line);
            // std::cout << "line \"" << line << "\"readed" << std::endl;
            content.push_back(line);
        }
        f.close();
        return content;
    }
    else
    {
        throw std::runtime_error("Cannot load postfix file " + fileName);
    }
}

std::size_t PSM::parseHeader(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex, const std::string &header)
{
    if (postfixFileContent.at(currentIndex) == header)
    {
        // std::cout << "Header parsed " << header << " line: " << currentIndex << std::endl;
        return ++currentIndex;
    }
    else
    {
        throw std::runtime_error("Expected " + header + " at line " + std::to_string(currentIndex) + " but " + postfixFileContent.at(currentIndex) + " met.");
    }
}

std::size_t PSM::parseCodeItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex)
{
    const auto items = splitStringToWords(postfixFileContent.at(currentIndex));
    if (items.size() != 2)
    {
        throw std::runtime_error("Expected 2 items at line " + std::to_string(currentIndex) + " but " + std::to_string(items.size()) + " met.");
    }
    else
    {
        _postfixCode.push_back({items[0], items[1]});
        return ++currentIndex;
    }
}

std::size_t PSM::parseVariableItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex)
{
    const auto items = splitStringToWords(postfixFileContent.at(currentIndex));
    if (items.size() != 2)
    {
        throw std::runtime_error("Expected 2 items at line " + std::to_string(currentIndex) + " but " + std::to_string(items.size()) + " met.");
    }
    else
    {
        const auto &variable = items[0];
        const auto &type = items[1];
        auto index = _tableOfId.size();
        _tableOfId[variable] = Data{index, type, std::monostate{}};
        return ++currentIndex;
    }
}

std::size_t PSM::parseConstansItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex)
{
    const auto items = splitStringToWords(postfixFileContent.at(currentIndex));
    if (items.size() != 2)
    {
        throw std::runtime_error("Expected 2 items at line " + std::to_string(currentIndex) + " but " + std::to_string(items.size()) + " met.");
    }
    else
    {
        const auto &valueStr = items[0];
        const auto &type = items[1];
        auto index = _tableOfConstants.size();
        auto value = Value{std::monostate{}};
        // PSM::Value value{std::monostate};
        if (type == "int")
        {
            value = std::stoi(valueStr);
        }
        else if (type == "float")
        {
            value = std::stof(valueStr);
        }
        _tableOfConstants[valueStr] = {index, type, value};
        return ++currentIndex;
    }
}

std::size_t PSM::parseSectionItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex, Section section)
{
    switch (section)
    {
    case Section::Code:
        return parseCodeItem(postfixFileContent, currentIndex);
    case Section::Variables:
        return parseVariableItem(postfixFileContent, currentIndex);
    case Section::Constants:
        return parseConstansItem(postfixFileContent, currentIndex);
    case Section::Labels:
        return parseLableItem(postfixFileContent, currentIndex);
    default:
        throw std::runtime_error("Unknown section");
    }
}

std::size_t PSM::parseSection(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex, Section section)
{
    // std::cout << "Parse section " << sectionHeaders[section] << std::endl;
    while (true)
    {
        auto nextStr = trim(postfixFileContent.at(currentIndex));
        ++currentIndex;
        if (nextStr.empty())
        {
            continue;
        }
        else if (nextStr == sectionHeaders[section])
        {
            break;
        }
        else
        {
            throw std::runtime_error("Expeted section header");
        }
    }
    while (true)
    {
        auto nextStr = trim(postfixFileContent.at(currentIndex));
        // std::cout << "next str: " << nextStr << std::endl;
        // ++currentIndex;
        if (nextStr.empty())
        {
            continue;
        }
        else if (nextStr == ")")
        {
            // std::cout << "Section " << sectionHeaders[section] << " parsed" << std::endl;
            break;
        }
        else
        {
            currentIndex = parseSectionItem(postfixFileContent, currentIndex, section);
        }
    }
    return ++currentIndex;
}

std::size_t PSM::parseLableItem(const std::vector<std::string> &postfixFileContent, std::size_t currentIndex)
{
    const auto items = splitStringToWords(postfixFileContent.at(currentIndex));
    if (items.size() != 2)
    {
        throw std::runtime_error("Expected 2 items at line " + std::to_string(currentIndex) + " but " + std::to_string(items.size()) + " met.");
    }
    else
    {
        _tableOfLabels[items[0]] = items[1];
        return ++currentIndex;
    }
}

void PSM::parsePostfixFileContent(std::vector<std::string> postfixFileContent)
{
    std::size_t currentIndex = 0;
    currentIndex = parseHeader(postfixFileContent, currentIndex, ".target: Postfix Machine");
    currentIndex = parseHeader(postfixFileContent, currentIndex, ".version: 0.2");
    currentIndex = parseSection(postfixFileContent, currentIndex, Section::Variables);
    currentIndex = parseSection(postfixFileContent, currentIndex, Section::Labels);
    currentIndex = parseSection(postfixFileContent, currentIndex, Section::Constants);
    currentIndex = parseSection(postfixFileContent, currentIndex, Section::Code);
}

void PSM::loadPostfixFile(const std::string &fileName)
{
    parsePostfixFileContent(readPostfixFileContent(fileName));
}

void PSM::printState() const
{
    // std::cout << "Table of variables" << std::endl;
    for (const auto &[name, data] : _tableOfId)
    {
        // std::cout << "\t" << name << ": index " << data._index << ", type " << data._type << ", value ";
        if (std::holds_alternative<std::monostate>(data._value))
        {
            std::cout << "undefined";
        }
        else if (std::holds_alternative<int>(data._value))
        {
            std::cout << std::to_string(std::get<int>(data._value));
        }
        else if (std::holds_alternative<float>(data._value))
        {
            std::cout << std::to_string(std::get<float>(data._value));
        }
        std::cout << std::endl;
    }
    std::cout << "Table of labels" << std::endl;
    for (const auto &[l, n] : _tableOfLabels)
    {
        std::cout << "\t" << l << " " << n << std::endl;
    }
    std::cout << "Table of constants" << std::endl;
    for (const auto &[name, data] : _tableOfConstants)
    {
        std::cout << "\t" << name << ": index " << data._index << ", type " << data._type << ", value ";
        if (std::holds_alternative<std::monostate>(data._value))
        {
            std::cout << "undefined";
        }
        else if (std::holds_alternative<int>(data._value))
        {
            std::cout << std::to_string(std::get<int>(data._value));
        }
        else if (std::holds_alternative<float>(data._value))
        {
            std::cout << std::to_string(std::get<float>(data._value));
        }
        std::cout << std::endl;
    }
    std::cout << "Code" << std::endl;
    for (std::size_t i = 0; i < _postfixCode.size(); ++i)
    {
        std::cout << "\t" << i << " "
                  << _postfixCode[i].first << " " << _postfixCode[i].second << std::endl;
    }
}

void PSM::printStack() const
{
    if (_stack.empty())
    {
        // std::cout << "Stack is empty" << std::endl;
    }
    else
    {
        // std::cout << "Stack content \n";
        std::stack<std::pair<std::string, std::string>> tempStack = _stack;

        // Iterate over the temporary stack and print its content
        while (!tempStack.empty())
        {
            // std::cout << tempStack.top().first << " " << tempStack.top().second << std::endl;
            tempStack.pop();
        }
    }
}

void PSM::doJump(const std::string &lex, const std::string &tok)
{
    const auto old = _currentInstructionIndex;
    // std::cout << "doJump" << std::endl;
    if (tok == "jump")
    {
        const auto &[lexLbl, _] = _stack.top();
        _stack.pop();
        _currentInstructionIndex = std::stoi(_tableOfLabels.at(lexLbl));
    }
    else if (tok == "colon")
    {
        _stack.pop();
        ++_currentInstructionIndex;
    }
    else if (tok == "jf")
    {
        const auto &[lexLbl, tokLbl] = _stack.top();
        _stack.pop();
        const auto &[valBool, _] = _stack.top();
        _stack.pop();
        if (valBool == "false")
        {
            _currentInstructionIndex = std::stoi(_tableOfLabels.at(lexLbl));
        }
        else
        {
            ++_currentInstructionIndex;
        }
    }
    // std::cout << "jump from " << std::to_string(old) << " to " << std::to_string(_currentInstructionIndex) << std::endl;
}

void PSM::doNot()
{
    auto [lex, tok] = _stack.top();
    _stack.pop();
    if (tok == "bool")
    {
        if (lex == "true")
        {
            _stack.push({"false", "bool"});
        }
        else
        {
            _stack.push({"true", "bool"});
        }
    }
    else
    {
        throw std::runtime_error("Bool expected for ! operator");
    }
}

void PSM::doNeg()
{
    std::string newL, newT;
    auto [lex, tok] = _stack.top();
    _stack.pop();
    if (tok == "r-val")
    {
        auto [_, type, value] = _tableOfId[lex];
        newT = type;
        if (type == "int")
        {
            newL = std::to_string(std::get<int>(value) * (-1));
        }
        else if (type == "float")
        {
            newL = std::to_string(std::get<float>(value) * (-1));
        }
    }
    else if (tok == "int")
    {
        newT = tok;
        newL = std::to_string(std::stoi(lex) * (-1));
    }
    else if (tok == "float")
    {
        newT = tok;
        newL = std::to_string(std::stof(lex) * (-1));
    }
    else
    {
        throw std::runtime_error("Unpredictable right token in neg operation: " + tok);
    }
    _stack.push({newL, newT});
}

std::pair<std::variant<int, float>, std::string> PSM::getValueAndType(const std::string &lex, const std::string &tok)
{
    if (tok == "int")
    {
        return {std::stoi(lex), "int"};
    }
    else if (tok == "float")
    {
        return {std::stof(lex), "float"};
    }
    else if (tok == "r-val")
    {
        auto [_, type, value] = _tableOfId[lex];
        if (std::holds_alternative<std::monostate>(value))
        {
            throw std::runtime_error("Undefined value of operand " + lex);
        }
        else
        {
            if (type == "int")
            {
                return {std::get<int>(value), type};
            }
            else // float
            {
                return {std::get<float>(value), type};
            }
        }
    }
    else
    {
        throw std::runtime_error("Unpredictible operand " + tok + " " + lex);
    }
}

void PSM::doAssign()
{
    auto [lexR, tokR] = _stack.top();
    _stack.pop();
    auto [lexL, tokL] = _stack.top();
    _stack.pop();
    if (tokL != "l-val")
    {
        throw std::runtime_error("Left operand in assign should be l-val but " + tokL + " " + lexL + " met at line " + std::to_string(_currentInstructionIndex));
    }
    if (tokR == "r-val")
    {
        auto [_, type, value] = _tableOfId[lexR];
        _tableOfId[lexL]._type = type;
        _tableOfId[lexL]._value = value;
    }
    else if (tokR == "int")
    {
        _tableOfId[lexL]._type = "int";
        _tableOfId[lexL]._value = std::stoi(lexR);
    }
    else if (tokR == "float")
    {
        _tableOfId[lexL]._type = "float";
        _tableOfId[lexL]._value = std::stof(lexR);
    }
    else
    {
        throw std::runtime_error("Unpredictable right token in assign operation: " + tokR);
    }
}

void PSM::doArithmetic(const std::string &lex, const std::string &tok)
{
    auto [lexR, tokR] = _stack.top();
    _stack.pop();
    auto [rValue, rType] = getValueAndType(lexR, tokR);
    auto [lexL, tokL] = _stack.top();
    _stack.pop();
    auto [lValue, lType] = getValueAndType(lexL, tokL);
    if (rType == "float" && lType == "int")
    {
        lType = "float";
        lValue = static_cast<float>(std::get<int>(lValue));
    }
    else if (rType == "int" && lType == "float")
    {
        rType = "float";
        rValue = static_cast<float>(std::get<int>(rValue));
    }
    auto resultType = rType;
    if (lex == "+")
    {
        if (resultType == "float")
        {
            _stack.push({std::to_string(std::get<float>(lValue) + std::get<float>(rValue)), resultType});
        }
        else
        { // int
            _stack.push({std::to_string(std::get<int>(lValue) + std::get<int>(rValue)), resultType});
        }
    }
    else if (lex == "-")
    {
        if (resultType == "float")
        {
            _stack.push({std::to_string(std::get<float>(lValue) - std::get<float>(rValue)), resultType});
        }
        else
        { // int
            _stack.push({std::to_string(std::get<int>(lValue) - std::get<int>(rValue)), resultType});
        }
    }
    else if (lex == "*")
    {
        if (resultType == "float")
        {
            _stack.push({std::to_string(std::get<float>(lValue) * std::get<float>(rValue)), resultType});
        }
        else
        { // int
            _stack.push({std::to_string(std::get<int>(lValue) * std::get<int>(rValue)), resultType});
        }
    }
    else if (lex == "^")
    {
        if (resultType == "float")
        {
            _stack.push({std::to_string(std::pow(std::get<float>(lValue), std::get<float>(rValue))), resultType});
        }
        else
        { // int
            _stack.push({std::to_string(std::pow(std::get<int>(lValue), std::get<int>(rValue))), resultType});
        }
    }
    else if (lex == "/")
    {
        if (resultType == "float")
        {
            auto r = std::get<float>(rValue);
            if (r == 0.0 || r == -0.0)
            {
                throw std::runtime_error("Ділення на 0");
            }
            _stack.push({std::to_string(std::get<float>(lValue) / r), "float"});
        }
        else
        { // int
            auto r = std::get<int>(rValue);
            if (r == 0)
            {
                throw std::runtime_error("Ділення на 0");
            }
            _stack.push({std::to_string(std::get<int>(lValue) / r), "int"});
        }
    }
}

void PSM::doBool(const std::string &lex, const std::string &tok)
{
    auto [lexR, tokR] = _stack.top();
    _stack.pop();
    auto [rValue, rType] = getValueAndType(lexR, tokR);
    auto [lexL, tokL] = _stack.top();
    _stack.pop();
    auto [lValue, lType] = getValueAndType(lexL, tokL);
    if (rType == "float" && lType == "int")
    {
        lType = "float";
        lValue = static_cast<float>(std::get<int>(lValue));
    }
    else if (rType == "int" && lType == "float")
    {
        rType = "float";
        rValue = static_cast<float>(std::get<int>(rValue));
    }
    auto resultType = rType;
    if (lex == ">")
    {
        if (resultType == "float")
        {
            _stack.push({std::get<float>(lValue) > std::get<float>(rValue) ? "true" : "false", "bool"});
        }
        else
        { // int
            _stack.push({std::get<int>(lValue) > std::get<int>(rValue) ? "true" : "false", "bool"});
        }
    }
    else if (lex == "<")
    {
        if (resultType == "float")
        {
            _stack.push({std::get<float>(lValue) < std::get<float>(rValue) ? "true" : "false", "bool"});
        }
        else
        { // int
            _stack.push({std::get<int>(lValue) < std::get<int>(rValue) ? "true" : "false", "bool"});
        }
    }
    else if (lex == ">=")
    {
        if (resultType == "float")
        {
            _stack.push({std::get<float>(lValue) >= std::get<float>(rValue) ? "true" : "false", "bool"});
        }
        else
        { // int
            _stack.push({std::get<int>(lValue) >= std::get<int>(rValue) ? "true" : "false", "bool"});
        }
    }
    else if (lex == "<=")
    {
        if (resultType == "float")
        {
            _stack.push({std::get<float>(lValue) <= std::get<float>(rValue) ? "true" : "false", "bool"});
        }
        else
        { // int
            _stack.push({std::get<int>(lValue) <= std::get<int>(rValue) ? "true" : "false", "bool"});
        }
    }
    else if (lex == "==")
    {
        if (resultType == "float")
        {
            _stack.push({std::get<float>(lValue) == std::get<float>(rValue) ? "true" : "false", "bool"});
        }
        else
        { // int
            _stack.push({std::get<int>(lValue) == std::get<int>(rValue) ? "true" : "false", "bool"});
        }
    }
    else if (lex == "!=")
    {
        if (resultType == "float")
        {
            _stack.push({std::get<float>(lValue) != std::get<float>(rValue) ? "true" : "false", "bool"});
        }
        else
        { // int
            _stack.push({std::get<int>(lValue) != std::get<int>(rValue) ? "true" : "false", "bool"});
        }
    }
    else
    {
        throw std::runtime_error("Unknown bool expression");
    }
}

void PSM::doIt(const std::string &lex, const std::string &tok)
{
    // std::cout << "do " << lex << " " << tok << std::endl;
    if (tok == "assign_op")
    {
        doAssign();
    }
    else if (tok == "neg")
    {
        doNeg();
    }
    else if (tok == "not_op")
    {
        doNot();
    }
    else if (arithmeticOperatorTokens.count(tok) != 0)
    {
        doArithmetic(lex, tok);
    }
    else if (boolOperatorTokens.count(tok) != 0)
    {
        doBool(lex, tok);
    }
}

void PSM::postfixExec()
{
    std::cout << "postfixExec:" << std::endl;
    _currentInstructionIndex = 0;
    _instructionsCount = _postfixCode.size();
    // std::cout << _currentInstructionIndex << " " << _instructionsCount << std::endl;
    while (_currentInstructionIndex < _instructionsCount)
    {
        printStack();
        const auto &[lex, tok] = _postfixCode.at(_currentInstructionIndex);
        // std::cout << lex << " " << tok << std::endl;
        if (valueTokens.count(tok) != 0)
        {
            _stack.push({lex, tok});
            ++_currentInstructionIndex;
        }
        else if (jumpTokens.count(tok) != 0)
        {
            doJump(lex, tok);
        }
        else if (tok == "out_op")
        {
            auto [lex, tok] = _stack.top();
            _stack.pop();
            auto [value, type] = getValueAndType(lex, tok);
            if (type == "int")
            {
                std::cout << std::to_string(std::get<int>(value)) << std::endl;
            }
            else
            {
                std::cout << std::to_string(std::get<float>(value)) << std::endl;
            }
            ++_currentInstructionIndex;
        }
        else if (tok == "in_op")
        {
            auto [lex, tok] = _stack.top();
            _stack.pop();
            std::string str;
            std::cout << "Enter number" << std::endl;
            std::cin >> str;
            auto [value, type] = parseNumber(str);
            _tableOfId[lex]._type = type;
            if (type == "int")
            {
                _tableOfId[lex]._value = std::get<int>(value);
            }
            else
            {
                _tableOfId[lex]._value = std::get<float>(value);
            }
            ++_currentInstructionIndex;
        }
        else
        {
            doIt(lex, tok);
            ++_currentInstructionIndex;
        }
    }
}