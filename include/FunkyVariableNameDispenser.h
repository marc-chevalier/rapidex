#pragma once

#include<vector>
#include<string>
#include<set>

#include "AbstractVariableNameDispenser.h"

class FunkyVariableNameDispenser : public AbstractVariableNameDispenser
{
public:
    FunkyVariableNameDispenser(){};
    virtual ~FunkyVariableNameDispenser(){};
    virtual std::vector<std::string> genNewVariables(size_t nbr, const std::set<std::string>& reserved);
    virtual std::string genNewVariable(const std::set<std::string>& reserved);
};
