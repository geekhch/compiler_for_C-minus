#ifndef LEX_PARSER_H
#define LEX_PARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <iterator>
#include <fstream>
#define BUFFER_SIZE 10240  //最大支持1MB源代码

using namespace std;

const string KEYW[] = {"else", "if", "int", "return", "void", "while"};
const char OPS[] = {'+', '-', '*', '/', '<', '=', '>', '!', ';', ',', '(', ')', '[', ']', '{', '}'};
const string TYPE_NAME[] = {"KEY", "INT", "ID", "OP", "PUNCT"};


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
    void putBackToken();

    //用于Token队列的保存和恢复
    uint32_t getMarker(){return cursor;}
    void toMarker(uint32_t marker){cursor = marker;}
    bool isEnd(){return cursor>=tokens.size();}
};


Token::Token(TYPE type, const long value, int line)
{
    this->type = type;
    this->i_value = value;
    this->line = line;
}

Token::Token(TYPE type, const string &value, int line)
{
    this->type = type;
    this->s_value = value;
    this->line = line;
}

string Token::strfToken()
{
    static char tokenString[100];
    if(type == INT)
    {
        sprintf(tokenString,"< %s   %d   %d>",TYPE_NAME[type].c_str(),i_value,line);
    }else
    {
        sprintf(tokenString,"< %s   %s   %d>",TYPE_NAME[type].c_str(),s_value.c_str(),line);
    }
    return string(tokenString);
}

LexParser::LexParser(const string &filepath)
{
    //将源代码读取到缓冲区
    f_code.open(filepath);
    char tmp;
    int cursor=0;
    while((tmp=f_code.get()) != EOF){
        if(cursor>BUFFER_SIZE-1){
            cout << "Buffer overflow!" << endl;
            exit(-1);
        }
        // 去掉注释
        if(cursor>0 && tmp=='*' && s_code[cursor-1]=='/'){
            cursor -= 1; //回退去掉前面的/注释标记
isEnd:      while((tmp=f_code.get()) != EOF && tmp!='*'){
                if(tmp=='\n')s_code[cursor++] = '\n';
            }
            if((tmp=f_code.get())!='/') goto isEnd;
            continue;
        }
        // 去掉换行符
        this->s_code[cursor++] = tmp;
    }
    s_code[cursor] = '\0';
    f_code.close();

    parseToken();
}

LexParser::~LexParser(){}

void LexParser::printCode() const
{
    cout << s_code << endl;
}

bool LexParser::isOperator(char c)
{
    for(char s:OPS){
        if(s==c) return true;
    }
    return false;
}

void LexParser::addTokenInt(const char *numStr, int line)
{
    char *endstring;
    long num = strtol(numStr, &endstring, 10);
    Token token(INT, num, line);
    tokens.push_back(token);
}

void LexParser::addTokenWord(const string &word, int line)
{
    //如果为保留字
    for(string w:KEYW){
        if(w==word){
            Token token(KEY, word, line);
            tokens.push_back(token);
            return;
        }
    }
    //不是保留字
    Token token(ID, word, line);
    tokens.push_back(token);
}

void LexParser::addTokenOp(const string &op, int line)
{
    Token token(OP, op, line);
    tokens.push_back(token);
}

void LexParser::parseToken()
{
    char TokenBuffer[60];
    int line = 1;
    for(int i=0; i<strlen(s_code); i++)
    {
        //跳过空白符
        if(s_code[i]=='\n') {++line;continue;}
        if(isblank(s_code[i])) continue;
        
        //进入int类型状态转换图
        if(isdigit(s_code[i])){
            int j=0;
            TokenBuffer[j++] = s_code[i];
            while(i<strlen(s_code)-1 && isdigit(s_code[i+1])) {TokenBuffer[j++] = s_code[i+1]; i++;}; //预读一个字符,并判断是否到达流尾
            if(isalpha(s_code[i+1])){
                TokenBuffer[j++] = s_code[i+1];
                TokenBuffer[j++] = '\0';
                char msg[100];
                sprintf(msg,"line %d: illegal lexical: %s",line, TokenBuffer);
                throw runtime_error(msg);
            } 
            TokenBuffer[j] = '\0';
            addTokenInt(TokenBuffer, line);
            continue;
        }
        //进入标识符和关键字状态转换图
        if(isalpha(s_code[i])){
            int j=0;
            TokenBuffer[j++] = s_code[i];
            while(i<strlen(s_code)-1 && isalpha(s_code[i+1])) {TokenBuffer[j++] = s_code[i+1]; i++;}; //预读一个字符,并判断是否到达流尾
            TokenBuffer[j] = '\0';
            addTokenWord(string(TokenBuffer), line);
            continue;
        }

        //操作符识别
        if(isOperator(s_code[i])){
            int j=0;
            TokenBuffer[j++] = s_code[i];
            if(s_code[i]=='<' || s_code[i]=='>' || s_code[i]=='!' || s_code[i]=='='){
                if(i<strlen(s_code)-1 && s_code[i+1]=='=') {TokenBuffer[j++] = s_code[i+1]; i++;};
            }
            TokenBuffer[j] = '\0';
            addTokenOp(string(TokenBuffer), line);
            continue;
        }

        //异常
        char msg[100];
        sprintf(msg,"line %d: thers is an illegal charactor:%c",line, s_code[i]);
        throw runtime_error(msg);
    }
}

void LexParser::printTokenList() const
{
    for(Token token: tokens){
        cout << token.strfToken() << endl;
    }
    cout << tokens.size() << " tokens in total!" << endl;
}

Token& LexParser::nextToken()
{
    if(cursor<tokens.size())
    {
        uint32_t next = cursor;
        ++cursor;
        return tokens[next];
    }else
    {
        throw runtime_error("no more tokens!"); //如果异常未被catch, 程序将递归终止
    }
}

void LexParser::putBackToken(){
    --cursor;
}

#endif
