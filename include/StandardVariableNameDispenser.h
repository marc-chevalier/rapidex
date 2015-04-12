#pragma once

#include "AbstractVariableNameDispenser.h"


class StandardVariableNameDispenser : public AbstractVariableNameDispenser
{
public:
    StandardVariableNameDispenser();
    virtual ~StandardVariableNameDispenser();
    virtual std::vector<std::string> genNewVariables(size_t nbr, const std::set<std::string>& reserved);
    virtual std::string genNewVariable(const std::set<std::string>& reserved);
};
