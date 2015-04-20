#include "../include/LatexPrinter.h"
#include<iostream>

using namespace std;

string genTitle();

LatexPrinter::LatexPrinter(const string& filename_, const string& compiler_) :
     filename(filename_), file(), isOpen(false), compiler(compiler_)
{}

LatexPrinter::~LatexPrinter()
{
    close();
}

void LatexPrinter::close()
{
    if(isOpen)
    {
        finaliseCodeMinimal();
        file.close();
        if(compiler != "")
            system((compiler+" "+filename+" > log.log").c_str());
    }
}

void LatexPrinter::open()
{
    file.open(filename);
    isOpen = true;
    initCodeMinimal();
}

void LatexPrinter::initCodeMinimal()
{
    file << "\\documentclass{article}\n\\usepackage[utf8]{inputenc}\n\\usepackage[usenames,dvipsnames]{xcolor}\\usepackage{booktabs}\n\\usepackage[francais]{babel}\n\\usepackage{amssymb}\n\\usepackage{amsmath}\n\\usepackage{color}\n\\title{"+genTitle()+"}\n\\author{Marc \\textsc{Chevalier}\\\\ Thomas \\textsc{Pellissier Tanon}}\n\\begin{document}\n\\maketitle\n";
}

void LatexPrinter::finaliseCodeMinimal()
{
    file << "\\end{document}\n";
}

void LatexPrinter::appendString(const string& str)
{
    if(!isOpen)
        open();
    file << str;
}

void LatexPrinter::appendLinearProgram(const string& linearProgramName, const LinearProgram& lp)
{
    if(!isOpen)
        open();
    LinearProgram::Objective objectiveType = lp.getObjectiveType();
    unordered_map<string, mpq_class> objectiveFunction (lp.getObjectiveFunction());

    vector<tuple<unordered_map<string, mpq_class>, LinearProgram::Relation, unordered_map<string, mpq_class>>>  constraints(lp.getConstraints());
    vector<tuple<string, LinearProgram::Relation, mpq_class>> bounds(lp.getBounds());

    vector<string> variableToColumnLhs;
    set<string> seenLhs;

    vector<string> variableToColumnRhs;
    set<string> seenRhs;

    for(tuple<unordered_map<string, mpq_class>, LinearProgram::Relation, unordered_map<string, mpq_class>> constraint : constraints)
    {
        for(pair<string, mpq_class> terme : get<0>(constraint))
        {
            if(seenLhs.count(terme.first) == 0 && terme.first != "")
            {
                variableToColumnLhs.push_back(terme.first);
                seenLhs.insert(terme.first);
            }
        }
        for(pair<string, mpq_class> terme : get<2>(constraint))
        {
            if(seenRhs.count(terme.first) == 0 && terme.first != "")
            {
                variableToColumnRhs.push_back(terme.first);
                seenRhs.insert(terme.first);
            }
        }
    }
    if(objectiveType == LinearProgram::MAXIMIZE)
        file << "Maximiser $";
    else
        file << "Minimiser $";

    bool first=true;
    for(pair<string, mpq_class> terme : objectiveFunction)
    {
        terme.second.canonicalize();
        if(terme.second > 0)
        {
             if(!first)
                file << " + ";
        }
        else if(terme.second < 0)
            file << " -";
        if(abs(terme.second) == 1)
            file << " "<< terme.first;
        else if(terme.second.get_den()!=1)
            file << " \\frac{" << abs(terme.second.get_num()) << "}{" << abs(terme.second.get_den()) << "}" << terme.first;
        else
            file << " " << abs(terme.second) << terme.first;
        first = false;
    }

    file << "$ tel que \n\n\\[\n"<<linearProgramName<<":\\qquad    \\left\\lbrace\\begin{array}{cr*{" << variableToColumnLhs.size() << "}{cr}ccr*{" << variableToColumnRhs.size() << "}{cr}}\n";
    for(tuple<unordered_map<string, mpq_class>, LinearProgram::Relation, unordered_map<string, mpq_class>> constraint : constraints)
    {
        first = true;
        if(get<0>(constraint).count("") != 0)
        {
            get<0>(constraint).at("").canonicalize();
            if(get<0>(constraint).at("") > 0)
                file << " &";
            else
                file << " -&";

            if(abs(get<0>(constraint).at("")) == 1)
                file << " 1";
            else if(get<0>(constraint).at("").get_den()!=1)
                file << " \\frac{" << abs(get<0>(constraint).at("").get_num()) << "}{" << abs(get<0>(constraint).at("").get_den()) << "}";
            else
                file << " " << abs(get<0>(constraint).at("")) << "";
            first = false;
        }
        else
            file << " &";

        for(string var : variableToColumnLhs)
        {
            if(get<0>(constraint).count(var) == 0)
            {
                file << " & & ";
                continue;
            }
            get<0>(constraint).at(var).canonicalize();
            if(get<0>(constraint).at(var) > 0)
            {
                if(first)
                    file << " &";
                else
                    file << " & +";
            }
            else
                file << " & -";

            if(abs(get<0>(constraint).at(var)) == 1)
                file << " & "<< var;
            else if(get<0>(constraint).at(var).get_den()!=1)
                file << " & \\frac{" << abs(get<0>(constraint).at(var).get_num()) << "}{" << abs(get<0>(constraint).at(var).get_den()) << "}" << var;
            else
                file << " & " << abs(get<0>(constraint).at(var)) << var;

            first = false;
        }

        if(first)
            file << "0";

        switch(get<1>(constraint))
        {
            case LinearProgram::LE : file << " & \\leqslant"; break;
            case LinearProgram::GE : file << " & \\geqslant"; break;
            case LinearProgram::EQ : file << " & = "; break;
            default : file << "";break;
        }

        first = true;
        if(get<2>(constraint).count("") != 0)
        {
            get<2>(constraint).at("").canonicalize();
            if(get<2>(constraint).at("") > 0)
                file << " &";
            else if(get<2>(constraint).at("") < 0)
                file << " & -";
            if(abs(get<2>(constraint).at("")) == 1)
                file << " & 1";
            else if(get<2>(constraint).at("").get_den()!=1)
                file << " & \\frac{" << abs(get<2>(constraint).at("").get_num()) << "}{" << abs(get<2>(constraint).at("").get_den()) << "}";
            else
                file << " & " << abs(get<2>(constraint).at(""));
            first = false;
        }
        else
            file << " & & ";


        for(string var : variableToColumnRhs)
        {
            if(get<2>(constraint).count(var) == 0)
            {
                file << " & & ";
                continue;
            }
            get<2>(constraint).at(var).canonicalize();
            if(get<2>(constraint).at(var) > 0)
            {
                if(first)
                    file << " &";
                else
                    file << " & +";
            }
            else if(get<2>(constraint).at(var) < 0)
                file << " & -";
            if(abs(get<2>(constraint).at(var)) == 1)
                file << " & "<< var;
            else if(get<2>(constraint).at(var).get_den()!=1)
                file << " & \\frac{" << abs(get<2>(constraint).at(var).get_num()) << "}{" << abs(get<2>(constraint).at(var).get_den()) << "}" << var;
            else
                file << " & " << abs(get<2>(constraint).at(var)) << var;

            first = false;
        }
        if(first)
            file << "0";
        file << "\\\\\n";
    }
    for(tuple<string, LinearProgram::Relation, mpq_class> bound : bounds)
    {
        for(int i=0;i<=static_cast<int>(2*(variableToColumnLhs.size()));++i)
            file << "&";
        file << get<0>(bound) << "";
        switch(get<1>(bound))
        {
            case LinearProgram::LE : file << " & \\leqslant"; break;
            case LinearProgram::GE : file << " & \\geqslant"; break;
            case LinearProgram::EQ : file << " & = "; break;
            default : file << "";break;
        }
        get<2>(bound).canonicalize();
        if(get<2>(bound) == 0)
            file << "&&0";
        else if(get<2>(bound) > 0)
        {
            if(first)
                file << " &";
        }
        else if(get<2>(bound) < 0)
            file << " & -";
        else if(get<2>(bound).get_den()!=1)
            file << " & \\frac{" << abs(get<2>(bound).get_num()) << "}{" << abs(get<2>(bound).get_den()) << "}";
        else
            file << " & " << abs(get<2>(bound));

        for(int i=0;i<=static_cast<int>(2*(variableToColumnRhs.size()))-2;++i)
            file << "&";
        file << "\\\\\n";
    }
    file << "    \\end{array}\\right.\n\\]\n";

}

void LatexPrinter::appendDict(const string& dictName, const Dictionary& dict)
{
    if(!isOpen)
        open();
    unordered_map<int, unordered_map<int, mpq_class>> dictionary(dict.getDictionary());
    unordered_map<int, mpq_class> objective(dict.getObjective());
    bool cte = false;
    bool x0 = false;
    vector<int> variableToColumn;
    set<int> seen;


    for(pair<int, unordered_map<int, mpq_class>> constraint : dictionary)
    {
        for(pair<int, mpq_class> terme : constraint.second)
        {
            if(seen.count(terme.first) == 0 && terme.first > 0)
            {
                variableToColumn.push_back(terme.first);
                seen.insert(terme.first);
            }
            else if(terme.first < 0)
                cte=true;
            else if(terme.first == 0)
                x0=true;
        }
    }

    file << "\\[\n"<<dictName<<":\\qquad    \\begin{array}{rc*{" << (variableToColumn.size()+(cte?1:0)+(x0?1:0)) << "}{cr}}\n";
    for(pair<int, unordered_map<int, mpq_class>> constraint : dictionary)
    {
        bool first = true;
        file << "        x_{" << constraint.first << "} & = ";
        if(constraint.second.count(-1) != 0)
        {
            constraint.second.at(-1).canonicalize();
            if(constraint.second.at(-1) > 0)
                file << " &";
            else if(constraint.second.at(-1) < 0)
                file << " & -";
            if(abs(constraint.second.at(-1)) == 1)
                file << " & 1";
            else if(constraint.second.at(-1).get_den()!=1)
                file << " & \\frac{" << abs(constraint.second.at(-1).get_num()) << "}{" << abs(constraint.second.at(-1).get_den()) << "}";
            else
                file << " & " << abs(constraint.second.at(-1));
            first = false;
        }
        else
            file << " & & ";

        for(int var : variableToColumn)
        {
            if(constraint.second.count(var) == 0)
            {
                file << " & & ";
                continue;
            }
            constraint.second.at(var).canonicalize();
            if(constraint.second.at(var) > 0)
            {
                if(first)
                    file << " &";
                else
                    file << " & +";
            }
            else if(constraint.second.at(var) < 0)
                file << " & -";
            if(abs(constraint.second.at(var)) == 1)
                file << " & x_{" << var << "}";
            else if(constraint.second.at(var).get_den()!=1)
                file << " & \\frac{" << abs(constraint.second.at(var).get_num()) << "}{" << abs(constraint.second.at(var).get_den()) << "}x_{" << var << "}";
            else
                file << " & " << abs(constraint.second.at(var)) << "x_{" << var << "}";

            first = false;
        }
        if(constraint.second.count(0) != 0)
        {
            constraint.second.at(0).canonicalize();
            if(constraint.second.at(0) > 0)
            {
                if(first)
                    file << " &";
                else
                    file << " & +";
            }
            else if(constraint.second.at(0) < 0)
                file << " & -";
            if(abs(constraint.second.at(0)) == 1)
                file << " & x_{" << 0 << "}";
            else if(constraint.second.at(0).get_den()!=1)
                file << " & \\frac{" << abs(constraint.second.at(0).get_num()) << "}{" << abs(constraint.second.at(0).get_den()) << "}" << "x_{" << 0 << "}";
            else
                file << " & " << abs(constraint.second.at(0)) << "x_{" << 0 << "}";
        }
        file << "\\\\\n";
    }
    file<<"        \\midrule\n";

    bool first = true;
    file << "        z & = ";
    if(objective.count(-1) != 0)
    {
        objective.at(-1).canonicalize();
        if(objective.at(-1) > 0)
            file << " &";
        else if(objective.at(-1) < 0)
            file << " & -";
        if(abs(objective.at(-1)) == 1)
            file << " & 1";
        else if(objective.at(-1).get_den()!=1)
            file << " & \\frac{" << abs(objective.at(-1).get_num()) << "}{" << abs(objective.at(-1).get_den()) << "}";
        else
            file << " & " << abs(objective.at(-1));
        first = false;
    }
    else
        file << " & & ";

    for(int var : variableToColumn)
    {
        if(objective.count(var) == 0)
        {
            file << " & & ";
            continue;
        }
        objective.at(var).canonicalize();
        if(objective.at(var) > 0)
        {
            if(first)
                file << " &";
            else
                file << " & +";
        }
        else if(objective.at(var) < 0)
            file << " & -";
        if(abs(objective.at(var)) == 1)
            file << " & x_{" << var << "}";
        else if(objective.at(var).get_den()!=1)
            file << " & \\frac{" << abs(objective.at(var).get_num()) << "}{" << abs(objective.at(var).get_den()) << "}x_{" << var << "}";
        else
            file << " & " << abs(objective.at(var)) << "x_{" << var << "}";

        first = false;
    }
    if(objective.count(0) != 0)
    {
        objective.at(0).canonicalize();
        if(objective.at(0) > 0)
        {
            if(first)
                file << " &";
            else
                file << " & +";
        }
        else if(objective.at(0) < 0)
            file << " & -";
        if(abs(objective.at(0)) == 1)
            file << " & x_{" << 0 << "}";
        else if(objective.at(0).get_den()!=1)
            file << " & \\frac{" << abs(objective.at(0).get_num()) << "}{" << abs(objective.at(0).get_den()) << "}" << "x_{" << 0 << "}";
        else
            file << " & " << abs(objective.at(0)) << "x_{" << 0 << "}";
    }
    file << "\\\\\n";
    file << "    \\end{array}\n\\]\n";
}
