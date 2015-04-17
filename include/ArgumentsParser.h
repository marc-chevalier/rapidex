#pragma once
#include<string>
#include<vector>
#include<unordered_map>

class ArgumentsParser
{
public:
    ArgumentsParser(std::vector<std::string> nomsOptions, std::vector<std::string> nomsParameters);
    ~ArgumentsParser(){};
    bool demandeAide();

    size_t nbArguments(){return arguments.size();};
    void parse(int argc, char* argv[]);


    std::string getArgument(size_t index) const;
    bool getOption(std::string nom) const;
    std::string getParameter(std::string nom) const;
    std::string getParameter(std::string nom, std::string def) const;

    static std::vector<std::string> getNomsOptions();
    static std::vector<std::string> getNomsParameters();

private:
    std::vector<std::string> arguments;
    std::unordered_map<std::string, bool> options;
    std::unordered_map<std::string, std::string> parameters;
};
