#pragma once
#ifndef YY_DECL
#define YY_DECL SimplexParser::Parser::token_type \
    SimplexParser::Lexer::yylex(SimplexParser::Parser::semantic_type* yylval, SimplexParser::Parser::location_type* yylloc)
#endif
#undef yyFlexLexer
#define yyFlexLexer SimplexParserFlexLexer_
#include <FlexLexer.h>

namespace SimplexParser {
    class Driver;
}

#include "SimplexParser.hpp"

namespace SimplexParser {
class Lexer : public SimplexParserFlexLexer_
{
public:
    Lexer(std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0);
    virtual ~Lexer();
    virtual Parser::token_type yylex(Parser::semantic_type *yylval, SimplexParser::Parser::location_type* yylloc);
    void set_debug(bool b);
};
}
