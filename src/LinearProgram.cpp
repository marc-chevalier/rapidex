#include "../include/LinearProgram.h"
#include "../include/LinearAlgebra.h"
#include<iostream>

using namespace std;

LinearProgram::LinearProgram() :
    objectiveType(MINIMIZE),
    objectiveTypeInit(MINIMIZE),
    objectiveFunction(unordered_map<string, mpq_class>()),
    constraints(vector<tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>>>()),
    variables(set<string>()),
    allVariables(set<string>()),
    sup(unordered_map<string, mpq_class>()),
    inf(unordered_map<string, mpq_class>()),
    variablesCorrespondence(unordered_map<string, int>()),
    reverseVariablesCorrespondence(vector<string>()),
	substs (unordered_map<string, unordered_map<string, mpq_class>>()),
	pseudoVariables (set<string>())
{}

LinearProgram::~LinearProgram()
{}

LinearProgram LinearProgram::getDual(AbstractVariableNameDispenser* varialbeNameDispenser)
{
    toCanonical();

    LinearProgram dual;
    size_t size = constraints.size();
    vector<string> dualVariables = varialbeNameDispenser->genNewVariables(size, variables);

    for(size_t i=0; i<size; ++i)
        if(get<1>(constraints[i]) == GE)
            dual.addBound(dualVariables[i], GE, 0);

    dual.setObjectiveType(LinearProgram::MINIMIZE);

    unordered_map<string, mpq_class> dualObjectiveFunction;

    for(size_t i=0; i<size; ++i)
        if(get<2>(constraints[i]).count("") != 0)
            dualObjectiveFunction[dualVariables[i]] = get<2>(constraints[i])[""];

    dual.setObjectiveFunction(dualObjectiveFunction);

    for(size_t i=0; i<size; ++i)
        if(get<1>(constraints[i]) == LE)
            dual.addBound(dualVariables[i], GE, 0);

    for(string variable : variables)
    {
        if(variable == "")
            continue;
        unordered_map<string, mpq_class> w;
        unordered_map<string, mpq_class> w_;

        if(objectiveFunction.count(variable) != 0)
            w_[""] = objectiveFunction.at(variable);

        for(size_t i=0; i<size; ++i)
            if(get<0>(constraints[i]).count(variable) != 0)
                w[dualVariables[i]] = get<0>(constraints[i]).at(variable);


        if(isPositive(variable))
            dual.addConstraint(w, GE, w_);
        else
            dual.addConstraint(w, EQ, w_);

        w.clear();
        w_.clear();
    }

    return dual;
}

bool LinearProgram::isPositive(const string& variable) const
{
    if(inf.count(variable) != 0)
        return inf.at(variable) >= 0;
    return false;
}

Dictionary LinearProgram::firstPhaseDictionary()
{
    unordered_map<int, unordered_map<int, mpq_class>> dictionary;
    unordered_map<int, mpq_class> objective;

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

    for(tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>> constraint : constraints)
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
    unordered_map<int, mpq_class> objective;
    unordered_map<int, unordered_map<int, mpq_class>> dictionary;

    for(pair<string, mpq_class> terme : objectiveFunction)
        if(terme.first != "")
            objective[variablesCorrespondence[terme.first]] = terme.second;
        else
            objective[-1] = terme.second;

    for(pair<int, unordered_map<int, mpq_class>> constraint : finalDictionary.getDictionary())
        objective = LinearAlgebra::substitution(objective, constraint.second, constraint.first);

    for(pair<int, unordered_map<int, mpq_class>> constraint : finalDictionary.getDictionary())
        for(pair<int, mpq_class> terme : constraint.second)
            if(terme.first != 0)
                dictionary[constraint.first][terme.first] = terme.second;

    objective.erase(0);
    LinearAlgebra::simplify(objective);

    return Dictionary(dictionary, objective);
}

mpq_class LinearProgram::applySubst(const unordered_map<int, mpq_class>& solution, const unordered_map<string, mpq_class>& subst) const
{
	mpq_class output = 0;
	if(subst.count("") != 0)
		output = subst.at("");

	for(pair<string, mpq_class> s : subst)
		if(s.first != "")
			output += s.second * solution.at(variablesCorrespondence.at(s.first));

	return output;
}

unordered_map<string, mpq_class> LinearProgram::getSolution(unordered_map<int, mpq_class> solution) const
{
#ifdef DEBUG
    for(pair<int, mpq_class> coord : solution)
        cout<<coord.first<<":"<<coord.second<<endl;
#endif

    unordered_map<string, mpq_class> output;

	for(pair<string, unordered_map<string, mpq_class>> subst : substs)
		output[subst.first] = applySubst(solution, subst.second);

	for(string pseudoVariable : pseudoVariables)
		solution.erase(variablesCorrespondence.at(pseudoVariable));


    size_t size = reverseVariablesCorrespondence.size();
	for(pair<int, mpq_class> coordonnee : solution)
        if(coordonnee.first >= 1 && static_cast<size_t>(coordonnee.first-1) < size)
            output[reverseVariablesCorrespondence[static_cast<size_t>(coordonnee.first-1)]] = coordonnee.second;

    return output;
}

pair<mpq_class, mpq_class> LinearProgram::applySubst(const unordered_map<int, pair<mpq_class, mpq_class>>& axis, const unordered_map<string, mpq_class>& subst) const
{
	pair<mpq_class, mpq_class> output = make_pair(0, 0);
	if(subst.count("") != 0)
		output.first = subst.at("");

	for(pair<string, mpq_class> s : subst)
		if(s.first != "")
		{
			output.first += s.second * axis.at(variablesCorrespondence.at(s.first)).first;
			output.second += s.second * axis.at(variablesCorrespondence.at(s.first)).second;
		}

	return output;
}

unordered_map<string, pair<mpq_class, mpq_class>> LinearProgram::getDivergenceAxis(unordered_map<int, pair<mpq_class, mpq_class>> axis) const
{
    unordered_map<string, pair<mpq_class, mpq_class>> output;

	for(pair<string, unordered_map<string, mpq_class>> subst : substs)
		output[subst.first] = applySubst(axis, subst.second);

	for(string pseudoVariable : pseudoVariables)
		axis.erase(variablesCorrespondence.at(pseudoVariable));

    size_t size = reverseVariablesCorrespondence.size();
	for(pair<int, pair<mpq_class, mpq_class>> coordonnee : axis)
        if(coordonnee.first >= 1 && static_cast<size_t>(coordonnee.first-1) < size)
            output[reverseVariablesCorrespondence[static_cast<size_t>(coordonnee.first-1)]] = coordonnee.second;

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
    size_t size = constraints.size();
    for(size_t i=0; i<size; ++i)
    {
        get<0>(constraints[i]) = LinearAlgebra::minus(get<0>(constraints[i]), get<2>(constraints[i]));
        get<2>(constraints[i]).clear();
    }
}

void LinearProgram::toLeftCteRight()
{
    size_t size = constraints.size();
    for(size_t i=0; i<size; ++i)
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
            constraints.push_back(tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>>(get<2>(constraints[i]), GE, get<0>(constraints[i])));
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
        if(inf.count(variable) != 0)
        {
            deletedVariables.insert(variable);
            newVariables.insert('^'+variable);
            inf['^'+variable] = 0;

            unordered_map<string, mpq_class> subst;
            subst['^'+variable] = 1;
            subst[""] = inf.at(variable);

            size_t size = constraints.size();
            for(size_t i=0;i<size;++i)
            {
                get<0>(constraints[i]) = LinearAlgebra::substitution(get<0>(constraints[i]), subst, variable);
                get<2>(constraints[i]) = LinearAlgebra::substitution(get<2>(constraints[i]), subst, variable);
            }
            objectiveFunction = LinearAlgebra::substitution(objectiveFunction, subst, variable);
            substs[variable] = subst;

            if(sup.count(variable) != 0)
            {
                sup['^'+variable] = sup.at(variable) - inf.at(variable);
                sup.erase(variable);
            }
            inf.erase(variable);
        }
        else if(sup.count(variable) != 0)
        {
            deletedVariables.insert(variable);
            newVariables.insert('_'+variable);
            inf['_'+variable] = 0;

            unordered_map<string, mpq_class> subst;
            subst['_'+variable] = -1;
            subst[""] = sup.at(variable);

            size_t size = constraints.size();
            for(size_t i=0;i<size;++i)
            {
                get<0>(constraints[i]) = LinearAlgebra::substitution(get<0>(constraints[i]), subst, variable);
                get<2>(constraints[i]) = LinearAlgebra::substitution(get<2>(constraints[i]), subst, variable);
            }
            objectiveFunction = LinearAlgebra::substitution(objectiveFunction, subst, variable);
            substs[variable] = subst;
            sup.erase(variable);
        }
        else
        {
            deletedVariables.insert(variable);
            newVariables.insert('^'+variable);
            newVariables.insert('_'+variable);
            inf['^'+variable] = 0;
            inf['_'+variable] = 0;

            unordered_map<string, mpq_class> subst;
            subst['^'+variable] = 1;
            subst['_'+variable] = -1;

            size_t size = constraints.size();
            for(size_t i=0;i<size;++i)
            {
                get<0>(constraints[i]) = LinearAlgebra::substitution(get<0>(constraints[i]), subst, variable);
                get<2>(constraints[i]) = LinearAlgebra::substitution(get<2>(constraints[i]), subst, variable);
            }
            objectiveFunction = LinearAlgebra::substitution(objectiveFunction, subst, variable);
            substs[variable] = subst;
        }
    }

    for(string variable : newVariables)
    {
        variables.insert(variable);
        pseudoVariables.insert(variable);
        allVariables.insert(variable);
    }

    for(string variable : deletedVariables)
        variables.erase(variable);
}

void LinearProgram::boundsToConstraints()
{
    for(pair<string, mpq_class> up : sup)
    {
        unordered_map<string, mpq_class> w;
        unordered_map<string, mpq_class> w_;
        w[up.first] = 1;
        w_[""] = up.second;
        constraints.push_back(tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>>(w, LE, w_));
    }
}

void LinearProgram::toAlmostCanonical()
{
#ifdef DEBUG
    cout<<"Initialement"<<endl;
    print();
#endif
    toPositiveVariables();
#ifdef DEBUG
    cout<<"Après toPositiveVariables"<<endl;
    print();
#endif
    toMaximization();
#ifdef DEBUG
    cout<<"Après toMaximization"<<endl;
    print();
#endif
    boundsToConstraints();
#ifdef DEBUG
    cout<<"Après boundsToConstraints"<<endl;
    print();
#endif
    toGE();
#ifdef DEBUG
    cout<<"Après toGE"<<endl;
    print();
#endif
    toLeft();
#ifdef DEBUG
    cout<<"Après toLeft (final)"<<endl;
    print();
#endif
}

void LinearProgram::toCanonical()
{
#ifdef DEBUG
    cout<<"Initialement"<<endl;
    print();
#endif
    toMaximization();
#ifdef DEBUG
    cout<<"Après toMaximization"<<endl;
    print();
#endif
    boundsToConstraints();
#ifdef DEBUG
    cout<<"Après boundsToConstraints"<<endl;
    print();
#endif
    toLEOrEQ();
#ifdef DEBUG
    cout<<"Après toLEOrEQ"<<endl;
    print();
#endif
    toLeftCteRight();
#ifdef DEBUG
    cout<<"Après toLeft (final)"<<endl;
    print();
#endif
}

vector<tuple<string, LinearProgram::Relation, mpq_class>>  LinearProgram::getBounds() const
{
    vector<tuple<string, LinearProgram::Relation, mpq_class>> output;

    for(pair<string, mpq_class> up : sup)
        output.push_back(tuple<string, Relation, mpq_class>(up.first, LE, up.second));

    for(pair<string, mpq_class> down : inf)
        output.push_back(tuple<string, Relation, mpq_class>(down.first, GE, down.second));

    return output;
}

void LinearProgram::addBound(const string& variable, Relation relation, mpq_class q)
{
    addVariable(variable);

    if(relation == GE)
    {
        if(inf.count(variable) == 0)
            inf[variable] = q;
        else
            inf[variable] = max(inf.at(variable), q);
    }
    else if(relation == EQ)
    {
        if(sup.count(variable) == 0)
            sup[variable] = q;
        else
            sup[variable] = min(sup.at(variable), q);
        if(inf.count(variable) == 0)
            inf[variable] = q;
        else
            inf[variable] = max(inf.at(variable), q);
    }
    else
    {
        if(sup.count(variable) == 0)
            sup[variable] = q;
        else
            sup[variable] = min(sup.at(variable), q);
    }
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

    addBound(variable, relation_, q);
}

void LinearProgram::addVariable(const string& str)
{
    if(str != "")
    {
        variables.insert(str);
        allVariables.insert(str);
    }
}

void LinearProgram::addVariable(const pair<string, mpq_class>& terme)
{
    if(terme.first != "")
    {
        variables.insert(terme.first);
        allVariables.insert(terme.first);
    }
}

void LinearProgram::addVariable(const unordered_map<string, mpq_class>& expr)
{
    for(pair<string, mpq_class> terme : expr)
        addVariable(terme);
}

void LinearProgram::addConstraint(const unordered_map<string, mpq_class>& lhs, Relation relation, const unordered_map<string, mpq_class>& rhs)
{
    addVariable(lhs);
    addVariable(rhs);

    constraints.push_back(tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>>(lhs, relation, rhs));
}

void LinearProgram::setObjectiveFunction(const unordered_map<string, mpq_class>& objectiveFunction_)
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
    for(pair<string, mpq_class> down : inf)
        cout << "  " << down.first << " >= " << down.second << endl;
    for(pair<string, mpq_class> up : sup)
        cout << "  " << up.first << " <= " << up.second << endl;
}

void LinearProgram::print(Objective obj) const
{
    if(obj==MAXIMIZE)
        cout<<"Maximiser ";
    else
        cout<<"Minimiser ";
}

void LinearProgram::print(const unordered_map<string, mpq_class>& dict) const
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

void LinearProgram::print(const vector<tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>>>& dict) const
{
    for(tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>> terme : dict)
        print(terme);
}

void LinearProgram::print(const tuple<unordered_map<string, mpq_class>, Relation, unordered_map<string, mpq_class>>& terme) const
{
    cout<<"  ";
    print(get<0>(terme));
    print(get<1>(terme));
    print(get<2>(terme));
    cout<<endl;
}
