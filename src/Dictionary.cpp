#include "../include/Dictionary.h"
#include "../include/LinearAlgebra.h"
#include<iostream>

using namespace std;

Dictionary::Dictionary(const unordered_map<int, unordered_map<int, mpq_class>>& dictionary_, const unordered_map<int, mpq_class>& objective_) :
    dictionary(dictionary_),
    objective(objective_)
{}

Dictionary::Dictionary() :
    dictionary(unordered_map<int, unordered_map<int, mpq_class>>()),
    objective(unordered_map<int, mpq_class>())
{}

Dictionary::~Dictionary()
{}

bool Dictionary::isFirstPhaseNeeded() const
{
    for(const pair<int, unordered_map<int, mpq_class>>& constraint : dictionary)
        if(constraint.second.count(-1) != 0)
            if(constraint.second.at(-1) < 0)
                return true;

    return false;
}

bool Dictionary::isSolved() const
{
    for(const pair<int, mpq_class>& terme : objective)
        if(terme.second > 0 && terme.first > 0)
            return false;
    return true;
}

mpq_class Dictionary::getSolutionValue() const
{
    if(objective.count(-1)==0)
        return 0;
    return objective.at(-1);
}

unordered_map<int, mpq_class> Dictionary::getSolutionPoint() const
{
    unordered_map<int, mpq_class> output;
    for(const pair<int, unordered_map<int, mpq_class>>& constraint : dictionary)
    {
        if(constraint.second.count(-1) == 0)
            output[constraint.first] = 0;
        else
            output[constraint.first] = constraint.second.at(-1);

        for(pair<int, mpq_class> terme : constraint.second)
            if(terme.first!=-1)
                output[terme.first] = 0;
    }
    return output;
}

pair<unordered_map<int, pair<mpq_class, mpq_class>>, pair<mpq_class, mpq_class>> Dictionary::getDivergenceAxis() const
{
    unordered_map<int, pair<mpq_class, mpq_class>> output;
    int divergenceVariable = getDivergenceVariable();

    for(pair<int, unordered_map<int, mpq_class>> constraint : dictionary)
    {
        if(constraint.second.count(-1) == 0)
            output[constraint.first].first = 0;
        else
            output[constraint.first].first = constraint.second.at(-1);

        for(pair<int, mpq_class> terme : constraint.second)
            if(terme.first!=-1)
            {
                output[terme.first].first = 0;
                output[constraint.first].second = 0;
            }


        if(constraint.second.count(divergenceVariable) == 0)
            output[constraint.first].second = 0;
        else
            output[constraint.first].second = constraint.second.at(divergenceVariable);
    }

    output[divergenceVariable].second = 1;

    return make_pair(output, make_pair(getSolutionValue(), objective.at(divergenceVariable)));
}

int Dictionary::getDivergenceVariable() const
{
    for(const pair<int, mpq_class>& terme : objective)
        if(terme.first > 0 && terme.second > 0 && isDivergenceVariable(terme.first))
            return terme.first;
    return -1;
}

bool Dictionary::isDivergenceVariable(int variable) const
{
    for(const pair<int, unordered_map<int, mpq_class>>& constraint : dictionary)
        if(constraint.second.count(variable) != 0)
            if(constraint.second.at(variable) < 0)
                return false;

    return true;
}

int Dictionary::getEnteringVariable() const
{
    int arginf = -1;
    for(const pair<int, mpq_class>& terme : objective)
        if(terme.second>0 && terme.first>0)
            if(arginf == -1 || arginf > terme.first)
                arginf = terme.first;

    return arginf;
}

int Dictionary::getLeavingInitialisationVariable() const
{
    mpq_class inf = 0;
    int arginf = -1;

    for(const pair<int, unordered_map<int, mpq_class>>& constraint : dictionary)
    {
        if(constraint.second.count(-1)==0)
        {
            if(0 < inf || arginf == -1)
            {
                arginf = constraint.first;
                inf = 0;
            }
        }

        if(constraint.second.at(-1) < inf || arginf == -1)
        {
            arginf = constraint.first;
            inf = constraint.second.at(-1);
        }
    }

    return arginf;
}

int Dictionary::getLeavingVariable(int enteringVariable) const
{
    mpq_class inf = -1;
    int arginf = -1;

    for(const pair<int, unordered_map<int, mpq_class>>& constraint : dictionary)
    {
        if(constraint.second.count(enteringVariable)==0)
            continue;

        if(constraint.second.at(enteringVariable)>=0)
            continue;

        mpq_class constant;
        if(constraint.second.count(-1)==0)
            constant = 0;
        else
            constant = constraint.second.at(-1);

        if(-constant/constraint.second.at(enteringVariable) < inf || arginf == -1)
        {
            arginf = constraint.first;
            inf = -constant/constraint.second.at(enteringVariable);
        }
    }

    return arginf;
}

unordered_map<int, unordered_map<int, mpq_class>> Dictionary::getDictionary() const
{
    return dictionary;
}

std::unordered_map<int, mpq_class> Dictionary::getObjective() const
{
    return objective;
}

void Dictionary::pivot(int enteringVariable, int leavingVariable)
{
    unordered_map<int, unordered_map<int, mpq_class>> newDictionary;
    unordered_map<int, mpq_class> isolation = isolateEnteringVariable(dictionary[leavingVariable], enteringVariable, leavingVariable);

    for(const pair<int, unordered_map<int, mpq_class>>& constraint : dictionary)
        if(constraint.first != leavingVariable)
            LinearAlgebra::substitution(newDictionary[constraint.first], constraint.second, isolation, enteringVariable);

    newDictionary[enteringVariable] = isolation;

    dictionary = newDictionary;

    LinearAlgebra::substitution(objective, isolation, enteringVariable);
}

unordered_map<int, mpq_class> Dictionary::isolateEnteringVariable(const unordered_map<int, mpq_class>& expression, int enteringVariable, int leavingVariable) const
{
    unordered_map<int, mpq_class> output;
    mpq_class coeffEnteringVariable = expression.at(enteringVariable);

    for(const pair<int, mpq_class>& terme : expression)
        if(terme.first != enteringVariable)
            output[terme.first] = -terme.second/coeffEnteringVariable;

    output[leavingVariable] = 1/coeffEnteringVariable;

    LinearAlgebra::simplify(output);
    return output;
}

void Dictionary::print() const
{
    for(const pair<int, unordered_map<int, mpq_class>>& c : dictionary)
    {
        cout<<"x_"<<c.first<<" = ";
        if(c.second.count(-1) != 0)
            cout<<c.second.at(-1)<<"  ";

        for(pair<int, mpq_class> terme : c.second)
        {
            if(terme.first == -1)
                continue;
            else if(terme.second>=0)
                cout<<"+"<<terme.second<<"*x_"<<terme.first<<"  ";
            else
                cout<<terme.second<<"*x_"<<terme.first<<" ";
        }
        cout<<endl;
    }
    cout<<"------------"<<endl;
    cout<<"z = ";
    if(objective.count(-1) != 0)
        cout<<objective.at(-1)<<"  ";
    for(pair<int, mpq_class> terme : objective)
    {
        if(terme.first==-1)
            continue;
        else if(terme.second>=0)
            cout<<"+"<<terme.second<<"*x_"<<terme.first<<"  ";
        else
            cout<<terme.second<<"*x_"<<terme.first<<" ";
    }
    cout<<endl;
}
