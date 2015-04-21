#pragma once
#include<unordered_map>
#include<vector>
#include<gmpxx.h>
#include<gmp.h>

class Dictionary
{
public:
    Dictionary();
    Dictionary(const std::unordered_map<int, std::unordered_map<int, mpq_class>>& dictionary, const std::unordered_map<int, mpq_class>& objective);
    ~Dictionary();
    bool isSolved() const;
    mpq_class getSolutionValue() const;
    std::unordered_map<int, mpq_class> getSolutionPoint() const;
    std::pair<std::unordered_map<int, std::pair<mpq_class, mpq_class>>, std::pair<mpq_class, mpq_class>> getDivergenceAxis() const;
    int getEnteringVariable() const;
    int getLeavingVariable(int enteringVariable) const;
    int getLeavingInitialisationVariable() const;
    std::unordered_map<int, std::unordered_map<int, mpq_class>> getDictionary() const;
    std::unordered_map<int, mpq_class> getObjective() const;
    bool isFirstPhaseNeeded() const;
    void pivot(int enteringVariable, int leavingVariable) __attribute__((hot));
    void print() const;

private:
    std::unordered_map<int, mpq_class> isolateEnteringVariable(const std::unordered_map<int, mpq_class>& expression, int enteringVariable, int leavingVariable) const;
    int getDivergenceVariable() const;
    bool isDivergenceVariable(int variable) const;

    std::unordered_map<int, std::unordered_map<int, mpq_class>> dictionary;
    std::unordered_map<int, mpq_class> objective;
};
