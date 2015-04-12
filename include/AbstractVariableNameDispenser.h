#pragma once

#include<set>
#include<string>
#include<vector>

class AbstractVariableNameDispenser
{
public:
    AbstractVariableNameDispenser();
    virtual ~AbstractVariableNameDispenser();
    virtual std::vector<std::string> genNewVariables(size_t nbr, const std::set<std::string>& reserved) = 0;
    virtual std::string genNewVariable(const std::set<std::string>& reserved) = 0;
};

class StandardVariableNameDispenser;
class FunkyVariableNameDispenser;
