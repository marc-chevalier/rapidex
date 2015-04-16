#pragma once
#include "Dictionary.h"
#include "AbstractVariableNameDispenser.h"
#include<string>
#include<tuple>
#include<set>

class LinearProgram
{
public:
    enum Objective
    {
        MINIMIZE,
        MAXIMIZE,
    };

    enum Relation
    {
        LE,
        GE,
        EQ,
    };
	
    LinearProgram();
    ~LinearProgram();

    LinearProgram getDual(AbstractVariableNameDispenser* varialbeNameDispenser);

    Dictionary firstPhaseDictionary();
    Dictionary secondPhaseDictionary(const Dictionary& dictionary);

    std::map<std::string, mpq_class> getSolution(const std::map<int, mpq_class>& solution) const;
    std::map<std::string, std::pair<mpq_class, mpq_class>> getDivergenceAxis(const std::map<int, std::pair<mpq_class, mpq_class>>& solution) const;
    mpq_class getOpt(mpq_class opt) const;

    void addBound(const std::string& variable, Relation relation, mpq_class q);
    void addBound(mpq_class q, Relation relation, const std::string& variable);
    void addConstraint(const std::map<std::string, mpq_class>& lhs, Relation relation, const std::map<std::string, mpq_class>& rhs);
    void setObjectiveFunction(const std::map<std::string, mpq_class>& objectiveFunction);
    void setObjectiveType(Objective objectiveType);
    void print() const;

    Objective getObjectiveType() const {return objectiveType;};
    std::map<std::string, mpq_class> getObjectiveFunction() const {return objectiveFunction;};
    std::vector<std::tuple<std::map<std::string, mpq_class>, Relation, std::map<std::string, mpq_class>>>  getConstraints() const {return constraints;};
    std::vector<std::tuple<std::string, Relation, mpq_class>> getBounds() const {return bounds;};

    std::set<std::string> getVariables() const {return variables;};

private:
    void print(Objective obj) const;
    void print(const std::map<std::string, mpq_class>& dict) const;
    void print(const std::vector<std::tuple<std::string, Relation, mpq_class>>& dict) const;
    void print(Relation rel) const;
    void print(const std::vector<std::tuple<std::map<std::string, mpq_class>, Relation, std::map<std::string, mpq_class>>>& dict) const;
    void print(const std::tuple<std::map<std::string, mpq_class>, Relation, std::map<std::string, mpq_class>>& dict) const;

    void toMaximization();
    void boundsToConstraints();
    void toLeft();
    void toLeftCteRight();
    void toGE();
    void toLEOrEQ();
    void toPositiveVariables();

    void addVariable(const std::string& str);
    void addVariable(const std::pair<std::string, mpq_class>& terme);
    void addVariable(const std::map<std::string, mpq_class>& expr);

    void toAlmostCanonical();
    void toCanonical();

    Objective objectiveType;
    Objective objectiveTypeInit;
    std::map<std::string, mpq_class> objectiveFunction;
    std::vector<std::tuple<std::map<std::string, mpq_class>, Relation, std::map<std::string, mpq_class>>> constraints;
    std::vector<std::tuple<std::string, Relation, mpq_class>> bounds;
    std::set<std::string> variables;
    std::map<std::string, bool> positiveness;
    std::map<std::string, int> variablesCorrespondence;
    std::vector<std::string> reverseVariablesCorrespondence;
	std::map<std::string, std::map<std::string, mpq_class>> substs;
	std::set<std::string> pseudoVariables;
};
