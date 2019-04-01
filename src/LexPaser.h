#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
using namespace std;

const char* KEYW[] = {"else","if","int","return","void","while"};
const char OP[] = {'+','-','*','/','<','=','>','!',';',',','(',')','[',']','{','}'};

typedef struct{
    string type;
    union{
         int i_num;
	 string s_key;
	 string s_id;
	 string s_op;
    };
}Token;

class LexPaser
{
private:
    vector<Token> tokens;
    FILE* fp;
public:
    LexPaser(char* filepath)
};
