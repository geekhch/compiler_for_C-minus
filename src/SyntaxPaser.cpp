#ifndef SYNTAX_PASER_H
#define SYNTAX_PASER_H

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include "LexPaser.h"
using namespace std;

struct SynNode//语法树结点
{
    bool isTerminal = false; 
    vector<SynNode> child; //子节点
    string word; //结点内容
};

//错误处理函数
void Error(string msg){cout << msg << endl; exit(-1);};

class SynPaser{
public:
    LexPaser &lex;
    SynPaser(LexPaser &lex_):lex(lex_){};

private:
    bool program();
    bool declaration_list();
    bool declaration();
    bool var_declaration();
    bool type_specifier();
    bool fun_declaration();
    bool params();
    bool param_list();
    bool param();
    bool compound_stmt();
    bool local_declarations();
    bool statement_list();
    bool statement();
    bool expression_stmt();
    bool selection_stmt();
    bool iteration_stmt();
    bool return_stmt();
    bool expression();
    bool var();
    bool simple_expression();
    bool relop();
    bool additive_expression();
    bool addop();
    bool term();
    bool mulop();
    bool factor();
    bool call();
    bool args();
    bool arg_list();
};

#endif