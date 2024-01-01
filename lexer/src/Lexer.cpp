#include <Lexer/Lexer.h>
using namespace std;

TableOfSymbols Lexer::getTableOfSymbols() const
{
    return tableOfSymb;
}

Lexer::Lexer()
{
    // Ініціалізація початкового стану та інших змінних
    state = initState;
    lenCode = 0;
    numLine = 1;
    numChar = -1;
    currentChar = '\0';
    lexeme = "";
    FSuccess = true;

    // δ - функція переходу станів
    this->stf = {
        {{0, 'L'}, 1},
        {{1, 'L'}, 1},
        {{1, 'D'}, 1},
        {{1, '_'}, 1},
        {{1, 'o'}, 2},
        {{0, 'D'}, 4},
        {{4, 'D'}, 4},
        {{4, 'd'}, 5},
        {{4, 'o'}, 9},
        {{5, 'D'}, 5},
        {{5, 'o'}, 6},
        {{0, '='}, 11},
        {{0, '+'}, 11},
        {{0, '-'}, 11},
        {{0, '*'}, 11},
        {{0, '/'}, 11},
        {{0, '>'}, 11},
        {{0, '<'}, 11},
        {{0, '!'}, 12},
        {{11, '='}, 12},
        {{11, 'o'}, 15},
        {{0, 'w'}, 0},
        {{0, 'n'}, 13},
        {{0, '('}, 14},
        {{0, ')'}, 14},
        {{0, '{'}, 14},
        {{0, '}'}, 14},
        {{0, 'o'}, 101}};
}

void Lexer::Analyze(const std::string &sourceCode)
{
    this->sourceCode = sourceCode;
    lenCode = sourceCode.length() - 1;

    try
    {
        while (numChar < lenCode)
        {
            currentChar = nextChar();                  // прочитати наступний символ
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

void Lexer::PrintResults() const
{
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
}

int Lexer::putCharBack(int numChar)
{
    return numChar - 1;
}
// Реалізація приватних методів класу Lexer
int Lexer::nextState(int state, char classCh)
{
    auto it = stf.find({state, classCh});
    if (it != stf.end())
    {
        return it->second;
    }
    else
    {
        return stf[{state, 'o'}]; // 'o' - дефолтний клас
    }
}

char Lexer::nextChar()
{
    numChar++;
    return sourceCode[numChar];
}

string Lexer::classOfChar(char ch)
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
    else if (string("+-=*/(){}><!_").find(ch) != string::npos)
    {
        return string(1, ch); // повертає рядок, що містить один символ ch
    }
    else
    {
        return "!";
    }
}

bool Lexer::is_final(int state)
{
    return F.find(state) != F.end();
}
string Lexer::getToken(int state, const string &lexeme)
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
int Lexer::indexIdConst(int state, const string &lexeme)
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
void Lexer::processing()
{
    if (state == 13)
    { // \n
        numLine++;
        state = initState;
    }

    if (state == 2 || state == 6 || state == 9 || state == 15)
    { // keyword, ident, float, int
        string token = getToken(state, lexeme);
        if (token != "keyword" && token != "bool")
        { // не keyword
            int index = indexIdConst(state, lexeme);
            cout << numLine << "\t" << lexeme << "\t" << token << "\t" << index << endl;
            tableOfSymb[nextSymbolIndex] = make_tuple(numLine, lexeme, token, index);
            ++nextSymbolIndex;
        }
        else
        { // якщо keyword
            cout << numLine << "\t" << lexeme << "\t" << token << endl;
            tableOfSymb[nextSymbolIndex] = make_tuple(numLine, lexeme, token, 0);
            ++nextSymbolIndex;
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
        tableOfSymb[nextSymbolIndex] = make_tuple(numLine, lexeme, token, 0);
        ++nextSymbolIndex;
        lexeme = "";
        state = initState;
    }

    if (Ferror.find(state) != Ferror.end())
    { // ERROR
        fail();
    }
}

void Lexer::fail()
{
    if (state == 101)
    {
        cout << "Lexer: у рядку " << numLine << " неочікуваний символ " << currentChar << endl;
        exit(101);
    }
    if (state == 102)
    {
        cout << "Lexer: у рядку " << numLine << " очікувався символ =, а не " << currentChar << endl;
        exit(102);
    }
}

TableOfConstants Lexer::getTableOfConstants() const
{
    return tableOfConst;
}
TableOfId Lexer::getTableOfId() const
{
    return tableOfId;
}
