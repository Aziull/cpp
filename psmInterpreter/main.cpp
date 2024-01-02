#include <PSM/PSM.h>

int main()
{
    try
    {
        PSM psm;
        psm.loadPostfixFile("TestData/test.postfix");
        psm.printState();
    }
    catch (const std::runtime_error &err)
    {
        std::cout << err.what() << std::endl;
    }
    return 0;
}