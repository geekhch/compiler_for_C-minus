#include "LexPaser.h"

int main(int argv, char* args[])
{
    if(argv<2){
	printf("use: %s filename",args[0]);
	exit(0);
    }
    //call lex paser

    //input commands
    printf("-c    print token numbers\n");
    printf("-t    print token lists");



    return 0;
}

