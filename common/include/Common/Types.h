#pragma once

#include <string>
#include <tuple>
#include <unordered_map>

using LineNumber = int;     // номер рядка програми;
using Lexeme = std::string; // лексема;4
using Token = std::string;  // токен лексеми;
using IdxIdConst = int;     // iндекс iдентифiкатора або константи у таблицi iдентифiкаторiв та констант вiдповiдно.
using TableOfSymbols = std::unordered_map<int, std::tuple<LineNumber, Lexeme, Token, IdxIdConst>>;

using TableOfConstants = std::unordered_map<std::string, std::pair<std::string, int>>;

using TableOfId = std::unordered_map<std::string, int>;

enum class ErrorType
{
    UnknownInstruction,
    UnpredictibleFileFinish,
    UndefinedVariable,
    UninitializedVariable
};
