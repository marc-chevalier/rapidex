#include "../include/StandardVariableNameDispenser.h"
#include<iostream>

using namespace std;

StandardVariableNameDispenser::StandardVariableNameDispenser()
{}

StandardVariableNameDispenser::~StandardVariableNameDispenser()
{}

vector<string> StandardVariableNameDispenser::genNewVariables(size_t nbr, const set<string>& reserved)
{
    vector<string> output;

    for(size_t i=0, j=0; i<nbr; ++j)
        if(reserved.count("y_"+to_string(j))==0)
        {
            output.push_back("y_"+to_string(j));
            ++i;
        }

    return output;
}

string StandardVariableNameDispenser::genNewVariable(const set<string>& reserved)
{
    string output;

    if(reserved.count("t")==0)
    {
        return "t";
    }

    for(size_t i=0, j=0; i<1; ++j)
        if(reserved.count("t_"+to_string(j))==0)
        {
            output = "t_"+to_string(j);
            ++i;
        }

    return output;
}
