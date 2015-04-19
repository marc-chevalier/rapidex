#include "../include/Simplex.h"
#include <iostream>

using namespace std;

Simplex::Simplex(LinearProgram program_, const string& filename, bool verboseLatex_, const string& compiler, bool verbose_) :
    program(program_), dictionary(Dictionary()), latex(filename, compiler), verboseLatex(verboseLatex_), verbose(verbose_)
{
#ifdef LATEX
    verboseLatex = true;
#endif
}

Simplex::~Simplex()
{}

mpq_class Simplex::getOpt() const
{
    return program.getOpt(dictionary.getSolutionValue());
}

unordered_map<string, mpq_class> Simplex::getSol() const
{
    return program.getSolution(dictionary.getSolutionPoint());
}

string Simplex::mpq_classToLatex(mpq_class rat) const
{
    rat.canonicalize();

    if(rat.get_den()==1)
        return rat.get_num().get_str(10);

    return "\\frac{"+rat.get_num().get_str(10)+"}{"+rat.get_den().get_str(10)+"}";
}

pair<unordered_map<string, pair<mpq_class, mpq_class>>, pair<mpq_class, mpq_class>> Simplex::divergenceAxis() const
{
    pair<unordered_map<int, pair<mpq_class, mpq_class>>, pair<mpq_class, mpq_class>> axis = dictionary.getDivergenceAxis();
    return make_pair(program.getDivergenceAxis(axis.first), axis.second);
}

Simplex::SolutionType Simplex::solve(const string& variableName)
{
    SolutionType solutionType = firstPhase();
    if(solutionType != SOLUTION)
        return solutionType;
    if(dictionary.getSolutionValue()!=0)
    {
        if(verboseLatex)
            latex.appendString("Le polytope est vide et il n'y a donc pas de solution.\n");
        return EMPTY;
    }
    if(verboseLatex)
        latex.appendString("Le polytope est non vide. On cherche la solution optimale.\n");
    solutionType = secondPhase();
    if(verboseLatex && solutionType == SOLUTION)
    {
        latex.appendString("La solution optimale est $"+mpq_classToLatex(getOpt())+"$ et est atteinte en \n\\[\n    \\begin{aligned}\n");

        unordered_map<string, mpq_class> sol = getSol();

        for(pair<string, mpq_class> coord : sol)
            latex.appendString("        "+coord.first+" &= "+mpq_classToLatex(coord.second)+"\\\\\n");
        latex.appendString("    \\end{aligned}\n\\]\n");
    }
    else if(verboseLatex && solutionType == UNBOUNDED)
    {
        pair<unordered_map<string, pair<mpq_class, mpq_class>>, pair<mpq_class, mpq_class>> axis = divergenceAxis();
        latex.appendString("Le domaine n'est pas borné. La droite $"+mpq_classToLatex(axis.second.first)+"+"+mpq_classToLatex(axis.second.second)+" \\times "+variableName+"$ est une solution divergente. Elle est obtenue pour\n\\[    \\begin{aligned}");

        for(pair<string, pair<mpq_class, mpq_class>> coord : axis.first)
            latex.appendString("        "+coord.first+" &= "+mpq_classToLatex(coord.second.first)+"+"+mpq_classToLatex(coord.second.second)+" \\times "+variableName+"\\\\\n");
        latex.appendString("    \\end{aligned}\n\\]\n");
    }

    return solutionType;
}

Simplex::SolutionType Simplex::firstPhase()
{
    int i=0;
    if(verboseLatex)
    {
        latex.appendString("\\section*{Systèmes}");
        latex.appendString("\\subsection*{Système initial}");
        latex.appendLinearProgram("(S)", program);
    }
    dictionary = program.firstPhaseDictionary();
    if(verboseLatex)
    {
        latex.appendString("\\subsection*{Système canonique}");
        latex.appendLinearProgram("(S_C)", program);
        latex.appendString("\\section*{Première phase}");
    }
#ifdef DEBUG
    cout<<"==Première phase=="<<endl;
    dictionary.print();
#endif
    if(!dictionary.isFirstPhaseNeeded())
    {
        if(verboseLatex)
            latex.appendString("La première phase n'est pas utile : 0 appartient au polytope. ");
#ifdef DEBUG
        cout<<"Inutile"<<endl;
#endif
        return SOLUTION;
    }

#ifdef DEBUG
    cout<<"Variable entrante : "<<0<<endl;
    cout<<"Variable sortante : "<<dictionary.getLeavingInitialisationVariable()<<endl;
#endif

    if(verboseLatex)
        latex.appendDict("(P'_{"+to_string(i)+"})", dictionary);
    ++i;
    if(verboseLatex)
        latex.appendString("$x_{0}$ entre et $x_{"+to_string(dictionary.getLeavingInitialisationVariable())+"}$ sort.");
    dictionary.pivot(0, dictionary.getLeavingInitialisationVariable());
    if(verboseLatex)
        latex.appendDict("(P'_{"+to_string(i)+"})", dictionary);
    ++i;
#ifdef DEBUG
    dictionary.print();
#endif
    while(!dictionary.isSolved())
    {
        int enteringVariable = dictionary.getEnteringVariable();
#ifdef DEBUG
        cout<<"Variable sortante: "<<enteringVariable<<endl;
#endif
        int leavingVariable = dictionary.getLeavingVariable(enteringVariable);
#ifdef DEBUG
        cout<<"Leaving variable: "<<leavingVariable<<endl<<endl;
#endif
        if(leavingVariable < 0)
            return UNBOUNDED;
        dictionary.pivot(enteringVariable, leavingVariable);
#ifdef DEBUG
        dictionary.print();
#endif
        if(verboseLatex)
        {
            latex.appendString("$x_{"+to_string(enteringVariable)+"}$ entre et $x_{"+to_string(leavingVariable)+"}$ sort.");
            latex.appendDict("(P'_{"+to_string(i)+"})", dictionary);
        }
        ++i;
    }
    return SOLUTION;
}

Simplex::SolutionType Simplex::secondPhase()
{
    int i=0;
    dictionary = program.secondPhaseDictionary(dictionary);
#ifdef DEBUG
    cout<<endl<<endl<<"==Seconde phase=="<<endl;
    dictionary.print();
#endif

    if(verboseLatex)
    {
        latex.appendDict("(P_{"+to_string(i)+"})", dictionary);
    }
    ++i;
    while(!dictionary.isSolved())
    {
        int enteringVariable = dictionary.getEnteringVariable();
#ifdef DEBUG
        cout<<"Variable entrante : "<<enteringVariable<<endl;
#endif
        int leavingVariable = dictionary.getLeavingVariable(enteringVariable);
#ifdef DEBUG
        cout<<"Variable sortante : "<<leavingVariable<<endl<<endl;
#endif
        if(leavingVariable < 0)
            return UNBOUNDED;
        dictionary.pivot(enteringVariable, leavingVariable);

#ifdef DEBUG
        dictionary.print();
#endif
        if(verboseLatex)
        {
            latex.appendString("$x_{"+to_string(enteringVariable)+"}$ entre et $x_{"+to_string(leavingVariable)+"}$ sort.");
            latex.appendDict("(P_{"+to_string(i)+"})", dictionary);
        }
        ++i;
    }
    return SOLUTION;
}
