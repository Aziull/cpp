#include <PSM/PSM.h>
#include <fstream>
#include <set>
#include <sstream>

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

    std::unordered_map<Section, std::string> sectionHeaders{{Section::Variables, ".vars("}, {Section::Labels, ".labels("}, {Section::Constants, ".constants("}, {Section::Code, ".code("}};

    std::set<std::string> valueTokens = {"int", "float", "l-val", "r-val", "label"};
    std::set<std::string> jumpTokens = {"jump", "jf", "colon"};
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
            std::cout << "line \"" << line << "\"readed" << std::endl;
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
        std::cout << "Header parsed " << header << " line: " << currentIndex << std::endl;
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
    std::cout << "Parse section " << sectionHeaders[section] << std::endl;
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
            std::cout << "Section " << sectionHeaders[section] << " parsed" << std::endl;
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
    std::cout << "Table of variables" << std::endl;
    for (const auto &[name, data] : _tableOfId)
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
    for (const auto &[l, t] : _postfixCode)
    {
        std::cout << "\t" << l << " "
                  << t << std::endl;
    }
}

void PSM::printStack() const
{
    if (_stack.empty())
    {
        std::cout << "Stack is empty" << std::endl;
    }
    else
    {
        std::cout << "Stack content \n";
        std::stack<std::pair<std::string, std::string>> tempStack = _stack;

        // Iterate over the temporary stack and print its content
        while (!tempStack.empty())
        {
            std::cout << tempStack.top().first << " " << tempStack.top().second << std::endl;
            tempStack.pop();
        }
    }
}

void PSM::doJump(const std::string &lex, const std::string &tok)
{
    const auto old = _currentInstructionIndex;
    std::cout << "doJump" << std::endl;
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
    std::cout << "jump from " << std::to_string(old) << " to " << std::to_string(_currentInstructionIndex) << std::endl;
}

void PSM::doIt(const std::string &lex, const std::string &tok)
{
    std::cout << "do " << lex << " " << tok << std::endl;
}

void PSM::postfixExec()
{
    std::cout << "postfixExec:" << std::endl;
    _currentInstructionIndex = 0;
    _instructionsCount = _postfixCode.size();
    std::cout << _currentInstructionIndex << " " << _instructionsCount << std::endl;
    while (_currentInstructionIndex < _instructionsCount)
    {
        printStack();
        const auto &[lex, tok] = _postfixCode.at(_currentInstructionIndex);
        std::cout << lex << " " << tok << std::endl;
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
            std::cout << "process out" << std::endl;
            ++_currentInstructionIndex;
        }
        else if (tok == "in_op")
        {
            std::cout << "process in" << std::endl;
            ++_currentInstructionIndex;
        }
        else
        {
            doIt(lex, tok);
            ++_currentInstructionIndex;
        }

        // print stack
        // push to stack int','float','l-val','r-val','label','bool'
        // process jumps
        // process out
        // process int
        // виконати арифметичні операції
    }
    // try:
    //   while self.numInstr < self.maxNumbInstr:
    //     self.stack.print()
    //     lex,tok = self.postfixCode[self.numInstr]
    //     if tok in ('int','float','l-val','r-val','label','bool'):
    //       self.stack.push((lex,tok))
    //       self.numInstr = self.numInstr +1
    //     elif tok in ('jump','jf','colon'):
    //       self.doJumps(lex,tok)
    //     elif tok == 'out_op':
    //       id, _ = self.stack.pop()
    //       self.numInstr = self.numInstr +1
    //       print(f'-------------- OUT: {id}={self.tableOfId[id][2]}')
    //     else:
    //       print(f'-=-=-=========({lex},{tok})  numInstr={self.numInstr}')
    //       self.doIt(lex,tok)
    //       self.numInstr = self.numInstr +1
    //   self.stack.print()
    // except PSMExcept as e:
    //   # Повідомити про факт виявлення помилки
    //   print('RunTime: Аварійне завершення програми з кодом {0}'.format(e))
}