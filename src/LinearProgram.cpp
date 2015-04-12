#include "../include/LinearProgram.h"
#include "../include/LinearAlgebra.h"
#include<iostream>

using namespace std;

LinearProgram::LinearProgram() :
    objectiveType(MINIMIZE),
    objectiveTypeInit(MINIMIZE),
    objectiveFunction(map<string, mpq_class>()),
    constraints(vector<tuple<map<string, mpq_class>, Relation, map<string, mpq_class>>>()),
    bounds(vector<tuple<string, Relation, mpq_class>>()),
    variables(set<string>()),
    positiveness(map<string, bool>()),
    variablesCorrespondence(map<string, int>()),
    reverseVariablesCorrespondence(vector<string>())
{}

LinearProgram::~LinearProgram()
{}

LinearProgram LinearProgram::getDual(AbstractVariableNameDispenser* varialbeNameDispenser)
{
    toCanonical();

    LinearProgram dual;
    vector<string> dualVariables = varialbeNameDispenser->genNewVariables(constraints.size(), variables);

    for(size_t i=0;i<constraints.size(); ++i)
        if(get<1>(constraints[i]) == GE)
            dual.addBound(dualVariables[i], GE, 0);

    dual.setObjectiveType(LinearProgram::MINIMIZE);

    map<string, mpq_class> dualObjectiveFunction;

    for(size_t i=0;i<constraints.size(); ++i)
        if(get<2>(constraints[i]).count("") != 0)
            dualObjectiveFunction[dualVariables[i]] = get<2>(constraints[i])[""];

    dual.setObjectiveFunction(dualObjectiveFunction);

    for(size_t i=0;i<constraints.size(); ++i)
        if(get<1>(constraints[i]) == LE)
            dual.addBound(dualVariables[i], GE, 0);

    for(string variable : variables)
    {
        if(variable == "")
            continue;
        map<string, mpq_class> w;
        map<string, mpq_class> w_;

        if(objectiveFunction.count(variable) != 0)
            w_[""] = objectiveFunction.at(variable);

        for(size_t i=0;i<constraints.size(); ++i)
            if(get<0>(constraints[i]).count(variable) != 0)
                w[dualVariables[i]] = get<0>(constraints[i]).at(variable);

        if(positiveness[variable])
            dual.addConstraint(w, GE, w_);
        else
            dual.addConstraint(w, EQ, w_);

        w.clear();
        w_.clear();
    }

    return dual;
}

Dictionary LinearProgram::firstPhaseDictionary()
{
    map<int, map<int, mpq_class>> dictionary;
    map<int, mpq_class> objective;

    toAlmostCanonical();
    int i = 1;
    for(string variable : variables)
    {
        if(variable == "")
            continue;
        variablesCorrespondence[variable] = i;
        reverseVariablesCorrespondence.push_back(variable);
        ++i;
    }

    for(tuple<map<string, mpq_class>, Relation, map<string, mpq_class>> constraint : constraints)
    {
        for(pair<string, mpq_class> terme : get<0>(constraint))
            if(terme.first == "")
                dictionary[i][-1] = terme.second;
            else
                dictionary[i][variablesCorrespondence[terme.first]] = terme.second;
        dictionary[i][0] = 1;
        ++i;
    }

    objective[0] = -1;

    return Dictionary(dictionary, objective);
}

Dictionary LinearProgram::secondPhaseDictionary(const Dictionary& finalDictionary)
{
    map<int, mpq_class> objective;
    map<int, map<int, mpq_class>> dictionary;

    for(pair<string, mpq_class> terme : objectiveFunction)
        objective[variablesCorrespondence[terme.first]] = terme.second;

    for(pair<int, map<int, mpq_class>> constraint : finalDictionary.getDictionary())
        objective = LinearAlgebra::substitution(objective, constraint.second, constraint.first);

    for(pair<int, map<int, mpq_class>> constraint : finalDictionary.getDictionary())
        for(pair<int, mpq_class> terme : constraint.second)
            if(terme.first != 0)
                dictionary[constraint.first][terme.first] = terme.second;

    objective[0] = 0;
    LinearAlgebra::simplify(objective);

    return Dictionary(dictionary, objective);
}

map<string, mpq_class> LinearProgram::getSolution(const map<int, mpq_class>& solution) const
{
    map<string, mpq_class> output;

    for(pair<int, mpq_class> coordonnee : solution)
        if(coordonnee.first >= 1 && static_cast<size_t>(coordonnee.first-1) < variablesCorrespondence.size())
        {
            string variableName = reverseVariablesCorrespondence[static_cast<size_t>(coordonnee.first-1)];
            if(variableName[0] == '+')
            {
                variableName = variableName.substr(1);
                output[variableName] = coordonnee.second - solution.at(variablesCorrespondence.at('-'+variableName));
            }
            else if(variableName[0] == '-')
                continue;
            else
                output[variableName] = coordonnee.second;
        }

    return output;
}

map<string, pair<mpq_class, mpq_class>> LinearProgram::getDivergenceAxis(const map<int, pair<mpq_class, mpq_class>>& solution) const
{
    map<string, pair<mpq_class, mpq_class>> output;

    for(pair<int, pair<mpq_class, mpq_class>> coordonnee : solution)
        if(coordonnee.first >= 1 && static_cast<size_t>(coordonnee.first-1) < variablesCorrespondence.size())
        {
            string variableName = reverseVariablesCorrespondence[static_cast<size_t>(coordonnee.first-1)];
            if(variableName[0] == '+')
            {
                variableName = variableName.substr(1);
                output[variableName] = make_pair(coordonnee.second.first - solution.at(variablesCorrespondence.at('-'+variableName)).first, coordonnee.second.second - solution.at(variablesCorrespondence.at('-'+variableName)).second);
            }
            else if(variableName[0] == '-')
                continue;
            else
                output[variableName] = make_pair(coordonnee.second.first, coordonnee.second.second);
        }

    return output;
}

mpq_class LinearProgram::getOpt(mpq_class opt) const
{
    return (objectiveTypeInit == MAXIMIZE) ? opt : static_cast<mpq_class>(-opt);
}

void LinearProgram::toMaximization()
{
    if(objectiveType == MINIMIZE)
    {
        objectiveType = MAXIMIZE;
        objectiveFunction = LinearAlgebra::scalarMultiplication(-1, objectiveFunction);
    }
}

void LinearProgram::toLeft()
{
    for(size_t i=0;i<constraints.size();++i)
    {
        get<0>(constraints[i]) = LinearAlgebra::minus(get<0>(constraints[i]), get<2>(constraints[i]));
        get<2>(constraints[i]).clear();
    }
}

void LinearProgram::toLeftCteRight()
{
    for(size_t i=0;i<constraints.size();++i)
    {
        get<0>(constraints[i]) = LinearAlgebra::minus(get<0>(constraints[i]), get<2>(constraints[i]));
        get<2>(constraints[i]).clear();
        if(get<0>(constraints[i]).count("")>0)
        {
            get<2>(constraints[i])[""] = - get<0>(constraints[i]).at("");
            get<0>(constraints[i]).erase("");
        }
    }
}

void LinearProgram::toGE()
{
    size_t size = constraints.size();

    for(size_t i=0;i<size;++i)
    {
        if(get<1>(constraints[i]) == LE)
        {
            get<0>(constraints[i]) = LinearAlgebra::scalarMultiplication(-1, get<0>(constraints[i]));
            get<1>(constraints[i]) = GE;
            get<2>(constraints[i]) = LinearAlgebra::scalarMultiplication(-1, get<2>(constraints[i]));
        }
        else if(get<1>(constraints[i]) == EQ)
        {
            get<1>(constraints[i]) = GE;
            constraints.push_back(tuple<map<string, mpq_class>, Relation, map<string, mpq_class>>(get<2>(constraints[i]), GE, get<0>(constraints[i])));
        }
    }
}

void LinearProgram::toLEOrEQ()
{
    size_t size = constraints.size();

    for(size_t i=0;i<size;++i)
    {
        if(get<1>(constraints[i]) == GE)
        {
            get<0>(constraints[i]) = LinearAlgebra::scalarMultiplication(-1, get<0>(constraints[i]));
            get<1>(constraints[i]) = LE;
            get<2>(constraints[i]) = LinearAlgebra::scalarMultiplication(-1, get<2>(constraints[i]));
        }
    }
}

void LinearProgram::toPositiveVariables()
{
    set<string> newVariables;
    set<string> deletedVariables;
    for(string variable : variables)
    {
        if(positiveness[variable] || variable=="")
            continue;
        deletedVariables.insert(variable);
        newVariables.insert('+'+variable);
        newVariables.insert('-'+variable);
        bounds.push_back(tuple<string, Relation, mpq_class>("+"+variable, GE, 0));
        bounds.push_back(tuple<string, Relation, mpq_class>("-"+variable, GE, 0));
        map<string, mpq_class> subst;
        subst['+'+variable] = 1;
        subst['-'+variable] = -1;

        size_t size = constraints.size();
        for(size_t i=0;i<size;++i)
        {
            get<0>(constraints[i]) = LinearAlgebra::substitution(get<0>(constraints[i]), subst, variable);
            get<2>(constraints[i]) = LinearAlgebra::substitution(get<2>(constraints[i]), subst, variable);
        }
        objectiveFunction = LinearAlgebra::substitution(objectiveFunction, subst, variable);
    }

    for(string variable : newVariables)
        variables.insert(variable);

    for(string variable : deletedVariables)
        variables.erase(variable);
}

void LinearProgram::boundsToConstraints()
{
    for(size_t i=0; i<bounds.size(); ++i)
    {
        if(!(get<1>(bounds[i]) == GE && get<2>(bounds[2]) == 0))
        {
            map<string, mpq_class> w;
            map<string, mpq_class> w_;
            w[get<0>(bounds[i])] = 1;
            w_[""] = get<2>(bounds[2]);
            constraints.push_back(tuple<map<string, mpq_class>, Relation, map<string, mpq_class>>(w, get<1>(bounds[i]), w_));
        }
    }
    bounds.clear();
    for(pair<string, bool> prop : positiveness)
        if(prop.second)
            bounds.push_back(tuple<string, Relation, mpq_class>(prop.first, GE, 0));
}

void LinearProgram::toAlmostCanonical()
{
    toMaximization();
    boundsToConstraints();
    toGE();
    toLeft();
    toPositiveVariables();
}

void LinearProgram::toCanonical()
{
    toMaximization();
    boundsToConstraints();
    toLEOrEQ();
    toLeftCteRight();
}

void LinearProgram::addBound(const string& variable, Relation relation, mpq_class q)
{
    addVariable(variable);
    if((relation == EQ || relation == GE) && q>=0)
        positiveness[variable] = true;
    bounds.push_back(tuple<string, Relation, mpq_class>(variable, relation, q));
}

void LinearProgram::addBound(mpq_class q, Relation relation, const string& variable)
{
    Relation relation_;
    if(relation==LE)
        relation_=GE;
    else if(relation==GE)
        relation_=LE;
    else
        relation_=relation;
    addVariable(variable);
    bounds.push_back(tuple<string, Relation, mpq_class>(variable, relation_, q));
}

void LinearProgram::addVariable(const string& str)
{
    if(str != "")
        variables.insert(str);
}

void LinearProgram::addVariable(const pair<string, mpq_class>& terme)
{
    if(terme.first != "")
        variables.insert(terme.first);
}

void LinearProgram::addVariable(const map<string, mpq_class>& expr)
{
    for(pair<string, mpq_class> terme : expr)
        addVariable(terme);
}

void LinearProgram::addConstraint(const map<string, mpq_class>& lhs, Relation relation, const map<string, mpq_class>& rhs)
{
    addVariable(lhs);
    addVariable(rhs);

    constraints.push_back(tuple<map<string, mpq_class>, Relation, map<string, mpq_class>>(lhs, relation, rhs));
}

void LinearProgram::setObjectiveFunction(const map<string, mpq_class>& objectiveFunction_)
{
    addVariable(objectiveFunction_);

    objectiveFunction = objectiveFunction_;
}

void LinearProgram::setObjectiveType(Objective objectiveType_)
{
    objectiveTypeInit = objectiveType_;
    objectiveType = objectiveType_;
}

void LinearProgram::print() const
{
    print(objectiveType);
    print(objectiveFunction);
    cout<<endl<<"Tel que :"<<endl;
    print(constraints);
    print(bounds);
    cout<<endl;
}

void LinearProgram::print(Objective obj) const
{
    if(obj==MAXIMIZE)
        cout<<"Maximiser ";
    else
        cout<<"Minimiser ";
}

void LinearProgram::print(const map<string, mpq_class>& dict) const
{
    bool init=true;

    if(dict.size()==0)
        cout<<"0";

    for(pair<string, mpq_class> terme : dict)
    {
        if(init)
        {
            if(terme.second!=1 || terme.first == "")
                cout<<terme.second;
            if(terme.second!=1 && terme.first!="")
                cout<<"*";
            if(terme.first!="")
                cout<<terme.first<<" ";

        }
        else
        {
            if(terme.second > 0)
                cout<<"+ ";
            else
                cout<<"- ";
            if(terme.second!=1 && terme.second!=-1)
            {
                if(terme.second > 0)
                    cout<<terme.second;
                else
                    cout<<-terme.second;
                cout<<"*";
            }
            if(terme.first!="")
                cout<<terme.first<<" ";
        }
        init=false;
    }
}

void LinearProgram::print(const vector<tuple<string, Relation, mpq_class>>& dict) const
{
    for(tuple<string, Relation, mpq_class> terme : dict)
    {
        cout<<"  "<<get<0>(terme);
        print(get<1>(terme));
        cout<<get<2>(terme)<<endl;
    }
}

void LinearProgram::print(Relation rel) const
{
    switch(rel)
    {
        case EQ: cout<<"=="; break;
        case LE: cout<<"<="; break;
        case GE: cout<<">="; break;
        default: cout<<""; break;
    }
}

void LinearProgram::print(const vector<tuple<map<string, mpq_class>, Relation, map<string, mpq_class>>>& dict) const
{
    for(tuple<map<string, mpq_class>, Relation, map<string, mpq_class>> terme : dict)
        print(terme);
}

void LinearProgram::print(const tuple<map<string, mpq_class>, Relation, map<string, mpq_class>>& terme) const
{
    cout<<"  ";
    print(get<0>(terme));
    print(get<1>(terme));
    print(get<2>(terme));
    cout<<endl;
}
