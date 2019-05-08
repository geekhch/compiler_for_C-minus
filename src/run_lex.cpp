#include "LexParser.hpp"

void stars();
void showMenu();

int main(int argv, char* args[])
{
    if(argv<2){
        printf("use: %s filename",args[0]);
        exit(0);
    }
    //call lex parser
    LexParser parser(args[1]);
    //input commands
    showMenu();
    stars();
    char cmd;
    while(cin>>cmd,cmd!='q')
    {
        switch(cmd)
        {
            case 'p': parser.printCode(); break;
            case 'h': showMenu(); break;
            case 't': parser.printTokenList();break;
            case 'n': cout<<parser.nextToken().strfToken()<<endl;break;
        }
        stars();
    }

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