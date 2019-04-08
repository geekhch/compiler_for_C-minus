
#ifndef LEX_PASER_H
#define LEX_PASER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#define BUFFER_SIZE 10240

using namespace std;

extern const string KEN[];
extern const char OPS[];
enum TYPE{KEY, INT, ID, OP, PUNCT};

class Token
{
public:
    TYPE type;
    int i_value;
    string s_value;


    Token(TYPE type, int value);
    Token(TYPE type, string value);
};

class LexPaser
{
private:
    vector<Token> tokens;
    ifstream f_code;
    void preParse();
    char s_code[BUFFER_SIZE]; //最大支持1MB源代码
    
public:
    LexPaser(const string&);
    ~LexPaser();
    void printCode();
};

#endif