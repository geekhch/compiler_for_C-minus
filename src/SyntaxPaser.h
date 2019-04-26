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
    bool programe();
    bool declaration_list();
    bool declaration();
    bool var_declaration();
    bool type_specifier();
};