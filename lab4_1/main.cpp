#include <Compiler/Compiler.h>
using namespace std;

int main()
{
    std::string fileName = "TestData/test.my_lang";
    Compiler compiler;
    compiler.compileToPostfix(fileName);
    return 0;
}