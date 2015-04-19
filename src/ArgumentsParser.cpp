#include<iostream>
#include <cstdlib>
#include "../include/ArgumentsParser.h"

using namespace std;

ArgumentsParser::ArgumentsParser(vector<string> nomsOptions, vector<string> nomsParameters) :
    arguments(vector<string>()), options(unordered_map<string, bool>()), parameters(unordered_map<string, string>())
{
    for(string option : nomsOptions)
        options[option] = false;

    for(string param : nomsParameters)
        parameters[param] = "";

    options["h"] = false;
    options["-help"] = false;
}

bool ArgumentsParser::demandeAide()
{
    return options["h"] || options["-help"];
}

void ArgumentsParser::parse(int argc, char* argv[])
{
    int compteurArguments = 0;
    for(int i = 1; i < argc; i++)
    {
        if( argv[i][0] == '-' )
        {
            string option = string(argv[i]).substr(1);
            if(option.find("=") == string::npos)
            {
                if(options.count(option) == 0)
                    cerr << "Option inconnue : " << option << endl;
                else
                options[option] = true;

            }
            else
            {
                if(parameters.count(option.substr(0,option.find("="))) == 0)
                    cerr << "Paramètre inconnu : " << option.substr(0,option.find("=")) << endl;
                else
                    parameters[option.substr(0,option.find("="))]=option.substr(option.find("=")+1);
            }
        }
        else
        {
            arguments.push_back(argv[i]);
            compteurArguments++;
        }
    }
}

string ArgumentsParser::getArgument(size_t index) const
{
    return arguments[index];
}

bool ArgumentsParser::getOption(string nom) const
{
    return options.at(nom);
}

string ArgumentsParser::getParameter(string nom) const
{
    return parameters.at(nom);
}

string ArgumentsParser::getParameter(string nom, string def) const
{
    if(parameters.count(nom)==0)
        return def;
    if(parameters.at(nom)=="")
        return def;
    return parameters.at(nom);
}

vector<string> ArgumentsParser::getNomsOptions()
{
    vector<string> liste(13);
    liste[0]="v";
    liste[1]="l";
    liste[2]="-latex";
    liste[3]="d";
    liste[4]="-dual";
    liste[5]="dr";
    liste[6]="-dual-resol";
    liste[7]="-funky";
    liste[8]="-tex";
    liste[9]="-dvi";
    liste[10]="-the";
    liste[11]="y";
    liste[12]="n";
    return liste;
}

vector<string> ArgumentsParser::getNomsParameters()
{
    vector<string> liste(1);
    liste[0]="output";
    return liste;
}
