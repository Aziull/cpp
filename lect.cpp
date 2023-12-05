#include <iostream>
#include <unordered_map>
#include <utility> // Для std::pair
#include <set>
#include <fstream>
#include <string>
using namespace std;

unordered_map<string, string> tokenTable = {
    {"program", "keyword"}, {"end", "keyword"},
    {"if", "keyword"}, {"else", "keyword"}, 
    {"for", "keyword"}, {":=", "assign_op"}, 
    {".", "dot"}, {" ", "ws"}, {"\t", "ws"},
    {"\n", "nl"}, {"-", "add_op"}, {"+", "add_op"},
    {"*", "mult_op"}, {"/", "mult_op"},
    {"(", "par_op"}, {")", "par_op"},
    {"^", "exp_op"}, {"!", "not_op"}, 
    {"<", "less_op"}, {">", "more_op"},
    {"!=", "not_eql_op"}, {"<=", "less_than_op"},
     {">=", "more_than_op"}};

// Решта токенів, визначених за заключним станом
unordered_map<int, string> tokStateTable = {
    {2, "ident"}, {6, "float"}, {9, "int"}};

// Тип для пари (стан, символ)
using StateSymbolPair = pair<int, char>;

// Функція для порівняння ключів у unordered_map
struct KeyHash
{
    size_t operator()(const StateSymbolPair &k) const
    {
        return hash<int>()(k.first) ^ hash<char>()(k.second);
    }
};

// Функція для порівняння значень у unordered_map
struct KeyEqual
{
    bool operator()(const StateSymbolPair &lhs, const StateSymbolPair &rhs) const
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

// δ - функція переходу станів
unordered_map<StateSymbolPair, int, KeyHash, KeyEqual> stf = {
    {{0, 'L'}, 1}, {{1, 'L'}, 1}, {{1, 'D'}, 1},
    {{1, 'o'}, 2}, {{0, 'D'}, 4}, {{4, 'D'}, 4},
    {{4, 'd'}, 5}, {{4, 'o'}, 9}, {{5, 'D'}, 5},
    {{5, 'o'}, 6}, {{0, ':'}, 11}, {{11, '='}, 12},
    {{11, 'o'}, 102}, {{0, 'w'}, 0}, {{0, 'n'}, 13},
    {{0, '+'}, 14}, {{0, '-'}, 14}, {{0, '*'}, 14}, 
    {{0, '/'}, 14}, {{0, '('}, 14}, {{0, ')'}, 14}, 
    {{0, 'o'}, 101}};

// Ініціалізація стартового стану
int initState = 0;

// Визначення заключних станів
set<int> F = {2, 6, 9, 10, 12, 13, 14, 101, 102};
set<int> Fstar = {2, 6, 9};   // Заключні стани, що використовуються у зірочці
set<int> Ferror = {101, 102}; // Стани для обробки помилок

// Таблиця ідентифікаторів
unordered_map<string, int> tableOfId;

// Таблиця констант. Припускаючи, що константи можуть мати тип та значення,
// ми використовуємо пару (тип, індекс) як значення.
unordered_map<string, pair<string, int>> tableOfConst;

// Таблиця символів програми. Це може бути, наприклад, таблиця для зберігання
// інформації про лексеми: номер рядка, лексему, тип токену та індекс.
// Тут використовується tuple для зберігання декількох значень.
unordered_map<int, tuple<int, string, string, int>> tableOfSymb;

int state = initState;

// Зчитування вмісту файлу
string sourceCode;

// Змінні для лексичного аналізу
bool FSuccess = true;    // Ознака успішності розбору (початково встановлюємо як true)
int lenCode;             // Номер останнього символа у файлі з кодом програми
int numLine = 1;         // Починаємо лексичний аналіз з першого рядка
int numChar = -1;        // Починаємо з першого символа
char currentChar = '\0'; // Поточний символ (ще не брали жодного символа)
string lexeme;           // Поточна лексема

void fail()
{
    if (state == 101)
    {
        cout << "Lexer: у рядку " << numLine << " неочікуваний символ " << int(currentChar) << endl;
        exit(101);
    }
    if (state == 102)
    {
        cout << "Lexer: у рядку " << numLine << " очікувався символ =, а не " << currentChar << endl;
        exit(102);
    }
}
bool is_final(int state)
{
    return F.find(state) != F.end();
}
int nextState(int state, char classCh)
{
    auto it = stf.find({state, classCh});
    if (it != stf.end())
    {
        return it->second;
    }
    else
    {
        return stf[{state, 'o'}];
    }
}
char nextChar()
{
    numChar++;
    return sourceCode[numChar];
}
int putCharBack(int numChar)
{
    return numChar - 1;
}
string classOfChar(char ch)
{
    if (ch == '.')
    {
        return "d";
    }
    else if (isalpha(ch) && islower(ch))
    { // перевірка на маленьку літеру
        return "L";
    }
    else if (isdigit(ch))
    { // перевірка на цифру
        return "D";
    }
    else if (ch == ' ' || ch == '\t')
    {
        return "w";
    }
    else if (ch == '\n')
    {
        return "n";
    }
    else if (string("+-:=*/()").find(ch) != string::npos)
    {
        return string(1, ch); // повертає рядок, що містить один символ ch
    }
    else
    {
        return "!";
    }
}

string getToken(int state, const string &lexeme)
{
    auto it = tokenTable.find(lexeme);
    if (it != tokenTable.end())
    {
        // Якщо ключ знайдений, повернути відповідне значення.
        return it->second;
    }
    else
    {
        // Якщо ключ не знайдений, спробувати знайти стан в tokStateTable.
        return tokStateTable[state];
    }
}

int indexIdConst(int state, const string &lexeme)
{
    int indx = 0;

    if (state == 2)
    {
        auto it = tableOfId.find(lexeme);
        if (it == tableOfId.end())
        {
            indx = tableOfId.size() + 1;
            tableOfId[lexeme] = indx;
        }
        else
        {
            indx = it->second;
        }
    }
    else if (state == 6 || state == 9)
    {
        auto it = tableOfConst.find(lexeme);
        if (it == tableOfConst.end())
        {
            indx = tableOfConst.size() + 1;
            tableOfConst[lexeme] = make_pair(tokStateTable[state], indx);
        }
        else
        {
            indx = it->second.second;
        }
    }

    return indx;
}
void processing()
{
    if (state == 13)
    { // \n
        numLine++;
        state = initState;
    }

    if (state == 2 || state == 6 || state == 9)
    { // keyword, ident, float, int
        string token = getToken(state, lexeme);
        if (token != "keyword")
        { // не keyword
            int index = indexIdConst(state, lexeme);
            cout << numLine << "\t" << lexeme << "\t" << token << "\t" << index << endl;
            tableOfSymb[numLine] = make_tuple(numLine, lexeme, token, index);
        }
        else
        { // якщо keyword
            cout << numLine << "\t" << lexeme << "\t" << token << endl;
            tableOfSymb[numLine] = make_tuple(numLine, lexeme, token, 0);
        }
        lexeme = "";
        numChar = putCharBack(numChar); // зірочка
        state = initState;
    }

    if (state == 12 || state == 14)
    {   
        lexeme += currentChar;
        string token = getToken(state, lexeme);
        cout << numLine << "\t" << lexeme << "\t" << token << endl;
        tableOfSymb[numLine] = make_tuple(numLine, lexeme, token, 0);
        lexeme = "";
        state = initState;
    }

    if (Ferror.find(state) != Ferror.end())
    { // ERROR
        fail();
    }
}
void lex()
{
    cout << "sourceCode: " << sourceCode << endl;

    try
    {
        while (numChar < lenCode)
        {
            currentChar = nextChar();             // прочитати наступний символ
            string classCh = classOfChar(currentChar); // до якого класу належить
            
            state = nextState(state, classCh[0]); // обчислити наступний стан
            if (F.find(state) != F.end())

            {                 // якщо стан заключний
                processing(); // виконати семантичні процедури
            }
            else if (state == initState)
            {
                lexeme = ""; // якщо стан НЕ заключний, а стартовий - нова лексема
            }
            else
            {
                lexeme += currentChar; // якщо стан НЕ закл. і не стартовий - додати символ до лексеми
            }
        }
        cout << "Lexer: Лексичний аналіз завершено успішно" << endl;
    }
    catch (int e)
    {
        // Встановити ознаку неуспішності
        FSuccess = false;
        // Повідомити про факт виявлення помилки
        cout << "Lexer: Аварійне завершення програми з кодом " << e << endl;
    }
}

int main()
{
    ifstream f("test.my_lang");
    if (f)
    {
        string line;
        while (getline(f, line))
        {
            sourceCode += line + "\n";
        }
        f.close();
    }
    lenCode = sourceCode.length() - 1;
    lex();
    // Виведення таблиць
    cout << string(30, '-') << endl;
    cout << "tableOfSymb:" << endl;
    for (const auto &item : tableOfSymb)
    {
        cout << "Key:\t" << item.first << "\tValue:("
             << get<0>(item.second) << ", " << get<1>(item.second) << ", "
             << get<2>(item.second) << ", " << get<3>(item.second) << ")" << endl;
    }

    cout << "tableOfId:" << endl;
    for (const auto &item : tableOfId)
    {
        cout << "Key:\t" << item.first << "\tValue:" << item.second << endl;
    }

    cout << "tableOfConst:" << endl;
    for (const auto &item : tableOfConst)
    {
        cout << "Key:\t" << item.first << "\tValue: ("
             << item.second.first << ", " << item.second.second << ")" << endl;
    }

    return 0;
}