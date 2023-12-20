#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <tuple>
#include <unordered_map>
#include <set>
#include <string>
#include "TokenTables.h" // Включення файлу з таблицями токенів
#include "Helpers.h"     // Включення допоміжних структур та функцій

using namespace std;

class Lexer
{
public:
    string sourceCode;
    Lexer();                                     // Конструктор
    void Analyze(const std::string &sourceCode); // Функція аналізу коду
    void PrintResults() const;                   // Функція для виведення результатів

private:
    std::unordered_map<string, pair<string, int> > tableOfConst;
    std::unordered_map<int, std::tuple<int, std::string, std::string, int> > tableOfSymb;
    std::unordered_map<string, int> tableOfId;
    std::unordered_map<StateSymbolPair, int, KeyHash, KeyEqual> stf;

    int initState = 0;
    int state;
    int lenCode;
    int numLine;
    int numChar = -1;
    char currentChar;
    string lexeme;
    bool FSuccess;

    // Допоміжні приватні методи
    void fail();
    bool is_final(int state);
    int nextState(int state, char classCh);
    char nextChar();
    string classOfChar(char ch);
    int putCharBack(int numChar);
    std::string getToken(int state, const std::string &lexeme);
    int indexIdConst(int state, const string &lexeme);
    void processing();
    // ... Інші допоміжні методи ...
};

#endif // LEXER_H