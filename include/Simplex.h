#pragma once
#include "LatexPrinter.h"

class Simplex
{
public:
    enum SolutionType
    {
        EMPTY,
        UNBOUNDED,
        SOLUTION,
    };
    Simplex(LinearProgram program, const std::string& filename, bool verboseLatex = false, const std::string& compiler = "pdflatex", bool verbose = false);
    ~Simplex();
    SolutionType solve(const std::string& variableName);
    mpq_class getOpt() const;
    std::unordered_map<std::string, mpq_class> getSol() const;
    std::pair<std::unordered_map<std::string, std::pair<mpq_class, mpq_class>>, std::pair<mpq_class, mpq_class>> divergenceAxis() const;

private:
    std::string mpq_classToLatex(mpq_class rat) const;

    SolutionType firstPhase();
    SolutionType secondPhase();

    LinearProgram program;
    Dictionary dictionary;
    LatexPrinter latex;
    bool verboseLatex;
    bool verbose;
};
