%code requires {
    #include "../include/Simplex.h"
}

%{

namespace SimplexParser {
    class Driver;
}

#include <sstream>
#include <iostream>
#include "SimplexParser.hpp"
#include "lexer.h"
#include "driver.h"
#include "../include/ParseError.h"
#define yylex driver.lexer->yylex
int	line_number	= 1;	/* number of current source line */


LinearProgram lp;

using namespace std;
%}

%union
{
    std::string* str;
    mpq_class* rat;
    LinearProgram* LP;
    std::pair<mpq_class, std::string>* t_terme;
    std::map<std::string, mpq_class>* t_list_terme;
    LinearProgram::Relation t_rel;
}

%token TEOF 0
%token INT
%token VARIABLE
%token GE LE EQ
%token MAXIMIZE MINIMIZE SUBJECT_TO BOUNDS VARIABLES EOL
%token PLUS MINUS FRAC TIMES

%type<str> VARIABLE
%type<rat> Signe INT ValAbs Coef
%type<t_terme> Terme InitTerme
%type<t_list_terme> ListTerm LinearCombination
%type<t_rel> Relation

%start Input

%language "C++"
%define namespace "SimplexParser"
%define parser_class_name "Parser"
%parse-param {Driver &driver}
%locations

%error-verbose

%%

Input:
Objective Constraints Bounds Variables TEOF { driver.setResult(lp); }
;

Objective:
 ObjectiveType EOL LinearCombination EOL { lp.setObjectiveFunction(*$3);}
;

ObjectiveType:
 MINIMIZE { lp.setObjectiveType(LinearProgram::MINIMIZE); }
|MAXIMIZE { lp.setObjectiveType(LinearProgram::MAXIMIZE); }
;

LinearCombination:
 InitTerme ListTerm { std::map<std::string, mpq_class>* p = $2;
                      (*p)[$1->second] += $1->first;
                      $$ = p;
                    }
;

ListTerm:
 ListTerm Terme { std::map<std::string, mpq_class>* p = $1;
                  (*p)[$2->second] += $2->first;
                  $$ = p;
                }
|               { $$ = new std::map<std::string, mpq_class>(); }
;

InitTerme:
 ValAbs                { $$ = new std::pair<mpq_class, std::string>(*$1, "");  }
|VARIABLE              { $$ = new std::pair<mpq_class, std::string>(1, *$1);   }
|ValAbs VARIABLE       { $$ = new std::pair<mpq_class, std::string>(*$1, *$2); }
|ValAbs TIMES VARIABLE { $$ = new std::pair<mpq_class, std::string>(*$1, *$3); }
|Terme                 { $$ = $1; }
;

Terme:
 Coef VARIABLE       { $$ = new std::pair<mpq_class, std::string>(*$1, *$2); }
|Coef TIMES VARIABLE { $$ = new std::pair<mpq_class, std::string>(*$1, *$3); }
|Coef                { $$ = new std::pair<mpq_class, std::string>(*$1, "" ); }
|Signe VARIABLE      { $$ = new std::pair<mpq_class, std::string>(*$1, *$2); }
;

Coef:
 Signe ValAbs { $$ = new mpq_class((*$1)*(*$2)); }
;

Signe:
 PLUS  { $$ = new mpq_class(1);  }
|MINUS { $$ = new mpq_class(-1); }
;

ValAbs:
 INT          { $$ = $1;                         }
|INT FRAC INT { $$ = new mpq_class((*$1)/(*$3)); }
;

Constraints:
 SUBJECT_TO EOL Constraints_list
|SUBJECT_TO Constraints_list
;

Constraints_list:
 Constraint EOL Constraints_list
|Constraint EOL
;

Constraint:
 LinearCombination Relation LinearCombination { lp.addConstraint(*$1, $2, *$3);}
;

Relation:
 LE { $$ = LinearProgram::LE; }
|GE { $$ = LinearProgram::GE; }
|EQ { $$ = LinearProgram::EQ; }
;

Bounds:
 BOUNDS EOL Bounds_list
|BOUNDS Bounds_list
|BOUNDS EOL
;

Bounds_list:
 Bound EOL Bounds_list
|Bound EOL
;

Bound:
 VARIABLE Relation INT { lp.addBound(*$1, $2, *$3); }
|INT Relation VARIABLE { lp.addBound(*$1, $2, *$3); }
|INT Relation VARIABLE Relation INT {
                                      lp.addBound(*$1, $2, *$3);
                                      lp.addBound(*$3, $4, *$5);
                                    }
;

Variables:
 VARIABLES EOL Variable_list
|VARIABLES Variable_list
;

Variable_list:
 VARIABLE EOL Variable_list
|VARIABLE EOL
|VARIABLE
;

%%

void SimplexParser::Parser::error(const location_type& loc, const std::string& m)
{
    int location = loc.begin.line;
    std::ostringstream out;
    out << "Ligne " << location << " : " << m;
    throw ParseError(out.str());
}
