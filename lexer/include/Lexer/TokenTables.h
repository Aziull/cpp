#ifndef TOKEN_TABLES_H
#define TOKEN_TABLES_H

#include <set>
#include <unordered_map>
#include <string>

using namespace std;

// Глобальна таблиця токенів
extern unordered_map<string, string> tokenTable;
// Таблиця станів токенів
extern unordered_map<int, string> tokStateTable;



extern set<int> F;
extern set<int> Fstar;   // Заключні стани, що використовуються у зірочці
extern set<int> Ferror; // Стани для обробки помилок

#endif // TOKEN_TABLES_H