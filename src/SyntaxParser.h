#ifndef SYNTAX_PARSER_H
#define SYNTAX_PARSER_H
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include "LexParser.h"
using namespace std;

struct SynNode//语法树结点
{
    string word; //结点内容
    vector<SynNode*> child; //子节点(为空时表示根节点，即终结符)

    // SynNode(string s):word(s){}
};

class SynParser{
public:
    SynNode *root;
    SynNode *curNode;
    LexParser &lex;

public:
    SynParser(LexParser &lex_):lex(lex_){
        root = new SynNode{"program"};
        curNode = root;
    }

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

bool SynParser::program()
{ //构造函数时已将program作为根节点
    if(declaration_list())
    {
        curNode = new SynNode{"declaration_list"};
        curNode->child.push_back(curNode);
        return true;
    }else{
        throw runtime_error("expected a declaration_list");
        return false;
    }
}
bool SynParser::declaration_list(){
    curNode = 
}
// bool SynParser::declaration(){}
// bool SynParser::var_declaration(){}
// bool SynParser::type_specifier(){}
// bool SynParser::fun_declaration(){}
// bool SynParser::params(){}
// bool SynParser::param_list(){}
// bool SynParser::param(){}
// bool SynParser::compound_stmt(){}
// bool SynParser::local_declarations(){}
// bool SynParser::statement_list(){}
// bool SynParser::statement(){}
// bool SynParser::expression_stmt(){}
// bool SynParser::selection_stmt(){}
// bool SynParser::iteration_stmt(){}
// bool SynParser::return_stmt(){}
// bool SynParser::expression(){}
// bool SynParser::var(){}
// bool SynParser::simple_expression(){}
// bool SynParser::relop(){}
// bool SynParser::additive_expression(){}
// bool SynParser::addop(){}
// bool SynParser::term(){}
// bool SynParser::mulop(){}
// bool SynParser::factor(){}
// bool SynParser::call(){}
// bool SynParser::args(){}
// bool SynParser::arg_list(){}