
#ifndef LEX_PARSER_H
#define LEX_PARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#define BUFFER_SIZE 10240  //最大支持1MB源代码

using namespace std;

extern const string KEN[];
extern const char OPS[];
enum TYPE{KEY, INT, ID, OP, PUNCT};

class Token
{
public:
    TYPE type;
    long i_value;
    uint16_t line;
    string s_value;


    Token(TYPE type, const long value, int line);
    Token(TYPE type, const string &value, int line);
    string strfToken();
};

/*
* LexParser每个对象解析一个c-文档，转换为tokens
*
*/
class LexParser
{
private:
    vector<Token> tokens;
    ifstream f_code;
    void preParse();
    char s_code[BUFFER_SIZE]; 
    uint32_t cursor = 0;

    //词法分析核心函数自动机解析Token
    void parseToken();
    
    //判断是否为C-的合法操作符
    bool isOperator(char);
    
    //将Token加入到队列
    void addTokenInt(const char *, int);
    void addTokenWord(const string &, int);
    void addTokenOp(const string &, int);

    
public:
    LexParser(const string&);
    ~LexParser();

    void printCode() const;
    void printTokenList() const;
    Token& nextToken();
};

#endif