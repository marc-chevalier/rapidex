#pragma once
#include<fstream>
#include"LinearProgram.h"

class LatexPrinter
{
public:
    LatexPrinter(const std::string& filename, const std::string& compiler);
    ~LatexPrinter();
    void appendDict(const std::string & dictName, const Dictionary& dict);
    void appendString(const std::string& str);
    void appendLinearProgram(const std::string& linearProgramName, const LinearProgram& lp);
    void close();
    void open();

private:
    void initCodeMinimal();
    void finaliseCodeMinimal();

    std::string filename;
    std::ofstream file;
    bool isOpen;
    std::string compiler;
};
