#pragma once
#include <istream>
#include "../include/LinearProgram.h"

namespace SimplexParser
{
class Parser;
class Lexer;

class Driver
{
public:
    Driver();
    Driver(const Driver& other);
    Driver& operator= (const Driver& other);
    ~Driver();

    LinearProgram parse(std::string& fileName);
    LinearProgram parse(std::istream& inputStream);

    void setResult(LinearProgram& result);
private:
    Lexer* lexer;
    Parser* parser;
    LinearProgram result;

    friend class Parser;
    friend class Lexer;
};
}
