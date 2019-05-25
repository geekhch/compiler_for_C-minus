#include "lexical/LexParser.hpp"
// #include "syntax/SynParserRec.hpp"
#include "syntax/SynParserLL1.hpp"
// #include "syntax/SynParserLR1.hpp"

void stars();
void showMenu();

int main(int argv, char* args[])
{
    if(argv<2){
        printf("use: %s filename",args[0]);
        exit(0);
    }
    //call lex parser
    LexParser lex_parser(args[1]);
    SynParser xyn_parser(lex_parser);
    return 0;
}

void stars()
{
    cout << endl << string(20,'*') << endl << endl;
}

void showMenu()
{
    printf("t    print token lists\n");
    printf("p    print the raw code\n");
    printf("h    print this showMenu\n");
    printf("n    print next token");
    printf("q    quit\n");
}