
#include "headers/TokenTables.h"


using namespace std;
// Глобальна таблиця токенів
unordered_map<string, string> tokenTable = {
    {"def", "keyword"}, {"return", "keyword"},
    {"if", "keyword"}, {"else", "keyword"},
    {"for", "keyword"}, {"=", "assign_op"}, 
    {".", "dot"}, {" ", "ws"}, {"\t", "ws"},
    {"\n", "nl"}, {"-", "add_op"}, 
    {"+", "add_op"}, {"*", "mult_op"},
    {"/", "mult_op"}, {"(", "par_op"},
    {")", "par_op"}, {"{", "cur_par_op"}, 
    {"}", "cur_par_op"}, {"^", "exp_op"}, 
    {"!", "not_op"}, {"<", "less_op"}, 
    {">", "more_op"}, {"!=", "not_eql_op"}, 
    {"<=", "less_than_op"}, {"!=", "not_op"}, 
    {"==", "eql_op"}, {">=", "more_than_op"},
    {"true", "bool"}, {"false", "bool"}};

// Таблиця станів токенів
unordered_map<int, string> tokStateTable = {
    {2, "ident"}, {6, "float"}, {9, "int"}
};

set<int> F = {2, 6, 9, 10, 12, 13, 14, 101, 15};
set<int> Fstar = {2, 6, 9, 15};   // Заключні стани, що використовуються у зірочці
set<int> Ferror = {101}; // Стани для обробки помилок