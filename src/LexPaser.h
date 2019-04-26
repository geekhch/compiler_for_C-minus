
#ifndef LEX_PASER_H
#define LEX_PASER_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

extern string KEN[];
extern char OP[];

typedef struct{
    string s_type; //指定
    unsigned int u_line;
    union{
        int i_num;
        string s_key;
        string s_id;
        string s_op;
    };
}Token;

/*
* LexPaser每个对象解析一个c-文档，转换为tokens
*
*/
class LexPaser
{
private:
    vector<Token> tokens;
    
public:
    LexPaser(const string &filepath);


};

#endif