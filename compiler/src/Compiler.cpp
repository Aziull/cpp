#include <Compiler/Compiler.h>
#include <Lexer/Lexer.h>
#include <Parser/Parser.h>
#include <fstream>

void Compiler::compileToPostfix(const std::string &fileName)
{
    std::cout << "compileToPostfix: Read source code " << std::endl;
    const auto sourceCode = readSourceCode(fileName);
    std::cout << "compileToPostfix: Lexer startup " << std::endl;
    Lexer lexer;
    lexer.Analyze(sourceCode);
    lexer.PrintResults();
    std::cout << "compileToPostfix: Startup parser (syntax alanysis + code generation)" << std::endl;
    Parser parser(lexer.getTableOfSymbols());
    parser.parse();
    std::cout << "compileToPostfix: Serve" << std::endl;
    serv(parser.getPostfixCode(), lexer.getTableOfId());
    std::cout << "compileToPostfix: Save postfix code" << std::endl;
    savePostfixCode(fileName, parser.getPostfixCode());
}

std::string Compiler::readSourceCode(const std::string &fileName)
{
    std::string sourceCode = "";
    std::ifstream f(fileName);
    if (f)
    {
        std::string line;
        while (getline(f, line))
        {
            sourceCode += line + "\n";
        }
        f.close();
    }
    std::cout << "source code " << sourceCode;
    return sourceCode;
}

void Compiler::savePostfixCode(const std::string &fileName, const std::vector<std::pair<std::string, std::string>> &postfixCode)
{
    const std::string ext = ".postfix";
    std::ofstream f(fileName + ext);
    if (f)
    {
        f << ".target: Postfix Machine\n.version: 0.2\n";

        f << "\n.code(\n";
        for (const auto &line : postfixCode)
        {
            f << "\t" << line.first << "\t" << line.second << "\n";
        }
        f << ")\n";
        f.close();
    }
    else
    {
        throw std::runtime_error("Cannot open file to store postfix code " + fileName);
    }
    std::cout << "postfix code збережено в файлі " << fileName << ext << std::endl;
}

void Compiler::serv(const std::vector<std::pair<std::string, std::string>> &postfixCode, const std::unordered_map<std::string, int> &tableOfId)
{
    std::cout << "Таблиця ідентифікаторів:" << std::endl;
    for (const auto &[str, i] : tableOfId)
    {
        std::cout << i << " " << str << std::endl;
    }
    std::cout << "Код програми у постфіксній формі (ПОЛІЗ):" << std::endl;
    for (std::size_t i = 0; i < postfixCode.size(); ++i)
    {
        std::cout << i << " " << postfixCode[i].first << " " << postfixCode[i].second << std::endl;
    }
}