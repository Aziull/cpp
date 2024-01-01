#include <iostream>
#include <unordered_map>
#include <utility> // Для std::pair
#include <set>
#include <fstream>
#include <Lexer/Lexer.h>
#include <Parser/Parser.h>
#include <string>
using namespace std;

int main()
{
    string sourceCode = "";
    ifstream f("TestData/test.my_lang");
    if (f)
    {
        string line;
        while (getline(f, line))
        {
            sourceCode += line + "\n";
        }

        f.close();
    }

    Lexer lexer;
    lexer.Analyze(sourceCode);
    Parser parser(lexer.getTableOfSymbols());
    parser.parse();
    return 0;
}