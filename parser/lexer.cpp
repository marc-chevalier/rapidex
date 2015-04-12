#include "lexer.h"

namespace SimplexParser {
Lexer::Lexer(std::istream* in, std::ostream* out) :
    SimplexParserFlexLexer_(in, out)
{}

Lexer::~Lexer()
{}

void Lexer::set_debug(bool b)
{
    yy_flex_debug = b;
}
}

#ifdef yylex
#undef yylex
#endif

int SimplexParserFlexLexer_::yylex()
{
    std::cerr << "Unexpected call to calcparserFlexLexer::yylex()!" << std::endl;
    return 0;
}

int SimplexParserFlexLexer_::yywrap()
{
    return 1;
}
