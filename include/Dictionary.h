#pragma once
#include<map>
#include<vector>
#include<gmpxx.h>
#include<gmp.h>

class Dictionary
{
public:
    Dictionary();
    Dictionary(const std::map<int, std::map<int, mpq_class>>& dictionary, const std::map<int, mpq_class>& objective);
    ~Dictionary();
    bool isSolved() const;
    mpq_class getSolutionValue() const;
    std::map<int, mpq_class> getSolutionPoint() const;
    std::pair<std::map<int, std::pair<mpq_class, mpq_class>>, std::pair<mpq_class, mpq_class>> getDivergenceAxis() const;
    int getEnteringVariable() const;
    int getLeavingVariable(int enteringVariable) const;
    int getLeavingInitialisationVariable() const;
    std::map<int, std::map<int, mpq_class>> getDictionary() const;
    std::map<int, mpq_class> getObjective() const;
    bool isFirstPhaseNeeded() const;
    void pivot(int enteringVariable, int leavingVariable);
    void print() const;

private:
    std::map<int, mpq_class> isolateEnteringVariable(const std::map<int, mpq_class>& expression, int enteringVariable, int leavingVariable) const;
    int getDivergenceVariable() const;
    bool isDivergenceVariable(int variable) const;

    std::map<int, std::map<int, mpq_class>> dictionary;
    std::map<int, mpq_class> objective;
};
