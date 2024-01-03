#include <PSM/PSM.h>
#include <Compiler/Compiler.h>

int main()
{
    try
    {
        std::string fileName = "TestData/test.my_lang";
        Compiler compiler;
        compiler.compileToPostfix(fileName);
        PSM psm;
        psm.loadPostfixFile(fileName + ".postfix");
        psm.printState();
        psm.postfixExec();
        psm.printState();
    }

    catch (const std::runtime_error &err)
    {
        std::cout << err.what() << std::endl;
    }
    return 0;
}