#include <iostream>
#include <unistd.h>
#include "../include/Simplex.h"
#include "../include/ParseError.h"
#include "../parser/driver.h"
#include "../include/ArgumentsParser.h"
#include "../include/StandardVariableNameDispenser.h"
#include "../include/FunkyVariableNameDispenser.h"

using namespace std;

LinearProgram parseLPFile(string fileName);
LinearProgram parseLPFile(string fileName)
{
    SimplexParser::Driver parserDriver;
    try
    {
        return parserDriver.parse(fileName);
    }
    catch(ParseError& e)
    {
        cerr << "Erreur du parser : " << e.getMessage() << "\nArrêt de la résolution." << endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    ArgumentsParser arguments(ArgumentsParser::getNomsOptions(), ArgumentsParser::getNomsParameters());
    arguments.parse(argc, argv);
    if(arguments.demandeAide())
    {
        cout<<endl<<"Syntaxes :"<<endl;
        cout<<"    ./rapidex file"<<endl<<endl;
        cout<<"-l / --latex             Détail de la résolution en LaTeX, par défaut, un fichier pdf est produit (cf. infra)"<<endl;
        cout<<"--output=file            Enregistrer le fichier latex dans file"<<endl;
        cout<<"--dvi                    Produit un fichier dvi à la place du pdf en utilisant latex"<<endl;
        cout<<"--tex                    Ne compile pas le .tex obtenu"<<endl<<endl;
        cout<<"-d / --dual              Afficher le dual"<<endl;
        cout<<"-dr / --dual-resol       Résoudre le dual au lieu du problème initial"<<endl;
        cout<<"--funky                  Amélioration du dual et de la détermination d'un axe divergent"<<endl;
        cout<<"-v / --verbose           Verbose"<<endl;
        cout<<"--the                    Essayez avec -v"<<endl;
        cout<<"--lait                   Essayez avec -v"<<endl;
        cout<<"-y / -n                  Essayez avec -v"<<endl<<endl;
        cout<<"-h / --help              Vous y êtes"<<endl<<endl;
        return EXIT_SUCCESS;
    }

    if(arguments.nbArguments()<1)
    {
        cout<<endl<<"Syntaxe :"<<endl;
        cout<<"./rapidex fichier"<<endl;
        cout<<"Faites `./rapidex --help' ou `./rapidex -h' pour plus d'informations."<<endl;
        return EXIT_FAILURE;
    }
    string message = "=======Résolution du problème primal=======";
    LinearProgram lp = parseLPFile(arguments.getArgument(0));
#ifdef DEBUG
    cout<<endl<<endl;
#endif
    if(arguments.getOption("v"))
    {
        cout<<"  Bonjour et bienvenue dans le mode verbeux."<<endl;
        cout<<"  Comment vous portez vous ?"<<endl;
        cout<<"  Vous prendrez bien le thé ? (--the)"<<endl;
        if(arguments.getOption("-the"))
        {
            cout<<"  Voici."<<endl;
            cout<<"  Je n'ai pas de sucre, désolé. Du lait ? (--lait)"<<endl;
            if(arguments.getOption("-lait"))
                cout<<"  Vous en avez assez ? Très bien."<<endl;
            cout<<"  Buvez, je vous en prie."<<endl;
            cout<<"  Absorbtion du thé : "<<endl;
            for(int i=0;i<10;++i)
            {
                /*int j;
                for(j=0;j<i;++j)
                    printf("*");
                for(;j<10;++j)
                    printf("-");
                usleep(100000);
                if(i<9)
                    for(j=0;j<11;++j)
                        printf("\b");*/
                cout<<"  ."<<endl;
                usleep(200000);
            }
            cout<<endl<<"  Était-il à votre goût ? (-y/-n)"<<endl;
            if(arguments.getOption("y"))
                cout<<"  J'en suis ravi. Au plaisir de vous revoir."<<endl;
            else if(arguments.getOption("n"))
                cout<<"  Oh ! Je suis navré. J'espère qu'il sera meilleur la prochaine fois."<<endl;
            else
                cout<<"  Quand dinerons nous chez vous ?"<<endl<<"    (pour mémoire : marc.chevalier@ens-lyon.org pour la réponse)"<<endl;
        }
        else
        {
            cout<<"  Quel dommage. Vous repasserez bien une autre fois."<<endl;
        }
        cout<<endl<<endl<<"==============Problème primal=============="<<endl<<endl;
        lp.print();
        cout<<            "==========================================="<<endl<<endl<<endl;
    }

    if(arguments.getOption("d") || arguments.getOption("-dual") || arguments.getOption("dr") || arguments.getOption("-dual-resol"))
    {
        AbstractVariableNameDispenser* nameDispenser;
        if(arguments.getOption("-funky"))
            nameDispenser = new FunkyVariableNameDispenser();
        else
            nameDispenser = new StandardVariableNameDispenser();

        LinearProgram dual = lp.getDual(nameDispenser);
        if((arguments.getOption("d") || arguments.getOption("-dual")) || (arguments.getOption("v") && !(arguments.getOption("dr") || arguments.getOption("-dual-resol"))) )
        {
            cout<<endl<<endl<<"===============Problème dual==============="<<endl<<endl;
            dual.print();
            cout<<            "==========================================="<<endl<<endl<<endl;
        }
        if(arguments.getOption("dr") || arguments.getOption("-dual-resol"))
        {
            message = "=======Résolution du problème dual=========";
            lp = dual;
        }
        delete nameDispenser;
    }

    string compiler = "pdflatex";
    if(arguments.getOption("-tex") && arguments.getOption("-dvi"))
    {
        cout<<"Options --tex et --dvi incompatibles"<<endl;
        return EXIT_FAILURE;
    }
    else if(arguments.getOption("-tex"))
        compiler = "";
    else if(arguments.getOption("-dvi"))
        compiler = "latex";


    Simplex simplex(lp, arguments.getParameter("output", arguments.getArgument(0)+".tex"), arguments.getOption("-latex")||arguments.getOption("l"), compiler, arguments.getOption("v"));

    AbstractVariableNameDispenser* nameDispenser;
    if(arguments.getOption("-funky"))
        nameDispenser = new FunkyVariableNameDispenser();
    else
        nameDispenser = new StandardVariableNameDispenser();
    string variableName = nameDispenser->genNewVariable(lp.getVariables());
    Simplex::SolutionType solutionType = simplex.solve(variableName);
    delete nameDispenser;

    cout<<endl<<endl<<message<<endl<<endl;
    if(solutionType == Simplex::SOLUTION)
    {
        cout<<"Solution = "<<simplex.getOpt()<<endl;
        unordered_map<string, mpq_class> solution(simplex.getSol());
        for(pair<string, mpq_class> coord : solution)
            cout<<"  "<<coord.first<<" = "<<coord.second<<endl;
    }
    else if(solutionType == Simplex::EMPTY)
    {
        cout<<"Le domaine est vide."<<endl;
    }
    else
    {
        cout<<"Le domaine n'est pas borné."<<endl;
        pair<unordered_map<string, pair<mpq_class, mpq_class>>, pair<mpq_class, mpq_class>> axis = simplex.divergenceAxis();
        cout<<"Axe de divergernce = "<<axis.second.first<<"+"<<axis.second.second<<"*"<<variableName<<endl;

        for(pair<string, pair<mpq_class, mpq_class>> coord : axis.first)
            cout<<"  "<<coord.first<<" = "<<coord.second.first<<"+"<<coord.second.second<<"*"<<variableName<<endl;

    }
    cout<<"==========================================="<<endl<<endl<<endl;

    return EXIT_SUCCESS;
}
