#ifndef SYN_PARSER_LL1_H
#define SYN_PARSER_LL1_H
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include <map>
#include <set>
#include <fstream>
#include <stack>
#include "LexParser.hpp"
using namespace std;

struct SynNode //语法树结点
{
    string word; //结点内容
    int type = -1;
    vector<SynNode *> children; //子节点(为空时表示根节点，即终结符)
};

struct ProUnit
{ //一个终结符或非终结符
    string word;
    int from=-1;
    bool isTerminal()
    {
        if (word == "NUM" || word == "ID" || word == "empty" || word == "$")
            return true;
        for (auto &s : KEYW)
        {
            if (s == word)
                return true;
        }
        for (auto &c : OPS)
        {
            if (c == word[0])
                return true;
        }
        return false;
    }

    //重载比较函数，与字符串比较
    bool operator==(const ProUnit o) const
    {
        return (this->word) == o.word;
    }
    bool operator<(const ProUnit o) const
    {
        return (this->word) < o.word;
    }
};

class SynParser
{
private:
    SynNode *root;
    LexParser &lex;
    vector<vector<ProUnit>> m_grammar;
    map<string, set<ProUnit>> FIRST, FOLLOW;
    map<string, map<string, int>> table; //非终结符-终结符-对应非终结符在grammar中的产生式下标

public:
    SynParser(LexParser &lex_); //读取产生式
private:
    bool getFIRSThelper(vector<ProUnit> &pd, int);
    void loadGrammar();
    void getFIRST();
    void getFOLLOW();
    void genParseTable(); //生成预测分析表
    void parse();
};

SynParser::SynParser(LexParser &lex_) : lex(lex_)
{
    loadGrammar();
    getFIRST();
    getFOLLOW();
    genParseTable();
    //输出分析表
    for(auto it=table.begin();it!=table.end();++it){
        cout << it->first<<endl;
        for(auto at=it->second.begin();at != it->second.end();++at){
            cout << at->first<<' '<<at->second<<"   ";
        }
        cout << endl;
    }
    
    //输出first follow语法结果
    cout <<"first:"<<endl;
    for(auto it=FIRST.begin();it!=FIRST.end();++it){
        cout <<it->first<<": ";
        for(auto u:it->second){
            cout <<u.word<<u.from<<" ";
        }
        cout << endl;
    }
    cout << endl <<"follow:"<<endl;
    for(auto it=FOLLOW.begin();it!=FOLLOW.end();++it){
        cout <<it->first<<": ";
        for(auto u:it->second){
            cout <<u.word<<" ";
        }
        cout << endl;
    }
}

void SynParser::loadGrammar()
{ //将文法产生式保存为map-vector数据结构
    fstream f_grm;
    f_grm.open("./syntax/grammar.txt");
    string word;

    vector<ProUnit> curPro;
    while (f_grm >> word)
    {
        curPro.push_back(ProUnit{word});

        if (f_grm.get() == '\n')
        { //一行结束，将当前符号加入对应非终结符的产生式
            m_grammar.push_back(curPro);
            curPro.clear();
        }
    }
}

bool SynParser::getFIRSThelper(vector<ProUnit> &pd, int from)
{//对一条产生式求first集
    string left = pd[0].word; //该条产生式左边的非终结符
    auto has_new = FIRST[left].size();
    for (auto it = pd.begin()+1; it != pd.end(); it++)
    {
        if (!it->isTerminal())
        {
            for(auto u: FIRST[it->word]){
                u.from = from;
                if(u.word!="empty" || it==pd.end()-1) //如果产生式推导出空
                    FIRST[left].insert(u);
            }
            //当前非终结符不能推出空停止扫描
            if (FIRST[it->word].count(ProUnit{"empty"}) == 0)
                break;
        }
        else
        { //遇到终结符停止扫描
            it->from = from;
            FIRST[left].insert(*it);
            break;
        }
    }
    return (has_new != FIRST[left].size());
}

void SynParser::getFIRST()
{
    //规则1，每条产生式首个终结符直接加入first
    bool more = true;
    while (more)
    {
        more = false;
        for (int i=0; i<m_grammar.size(); ++i)
        {
            more |= getFIRSThelper(m_grammar[i], i);
        }
    }
}

void SynParser::getFOLLOW()
{
    //规则1：开始符号加入$
    FOLLOW["program"].insert(ProUnit{"$"});
    //规则2 3

    bool more = true;
    while (more)
    {
        more = false;
        for (auto pd : m_grammar)
        {   
            //规则2 对一个产生式中的所有非终结符，将后面的first集加入其follow集
            for (int i = 1; i < pd.size() - 1; ++i)
            {//从前往后
                if (!pd[i].isTerminal())
                { // 找到一个非终结符pd[i]
                    string noTerminal = pd[i].word;
                    auto hasMore = FOLLOW[noTerminal].size();
                    for (int j = i + 1; j < pd.size(); ++j)
                    {
                        if (pd[j].isTerminal())
                            FOLLOW[noTerminal].insert(pd[j]);
                        else
                        {   //后面的非终结符first加入noTerminal对应follow集合
                            FOLLOW[noTerminal].insert(FIRST[pd[j].word].begin(), FIRST[pd[j].word].end());
                            if (FIRST[pd[j].word].count(ProUnit{"empty"}) == 0)
                                break;
                        }
                    }
                    more |= (hasMore != FOLLOW[noTerminal].size());
                }
            }

            //规则3 以非终结符结尾的，将左部的follow集并入末尾非终结符
            for (int j = pd.size() - 1; j > 0; j--)
            {//从后往前
                if (pd[j].isTerminal())
                    break;

                auto hasMore = FOLLOW[pd[j].word].size();
                FOLLOW[pd[j].word].insert(FOLLOW[pd[0].word].begin(), FOLLOW[pd[0].word].end());
                more |= (hasMore != FOLLOW[pd[j].word].size());

                if (FIRST[pd[j].word].count(ProUnit{"empty"}) == 0)
                    break; //不含空集，不再往前推导
            }
        }
    }
    //去掉空符号
    for (auto it = FOLLOW.begin(); it != FOLLOW.end(); ++it)
    {
        it->second.erase(ProUnit{"empty"});
    }
}

void SynParser::genParseTable()
{
    for(auto it=FIRST.begin(); it!=FIRST.end(); ++it){
        for(ProUnit fst: it->second){
            if(fst.word!="empty"){
                if(table[it->first].count(fst.word)>0)
                    throw runtime_error("预测分析表产生式冲突！");
                table[it->first][fst.word] = fst.from;
            }else{//有空符号，则操作follow集
                for(ProUnit flw:FOLLOW[it->first]){
                    if(table[it->first].count(fst.word)>0)
                        throw runtime_error("预测分析表产生式冲突！");
                    table[it->first][flw.word] = fst.from;
                }
            }
        }
    }
}

// void SynParser::parse(){
//     stack<ProUnit> S; //符号栈
//     S.push(ProUnit{"$"});
//     S.push(ProUnit{"program"});
//     ProUnit u = S.top();  //栈顶符号
//     vector<vector<ProUnit>> out;
//     while(u.word != "$"){
//         uint32_t mk = lex.getMarker();
//         Token t = lex.nextToken();
//         if(u.isTerminal()){ //栈顶终结符匹配
//             if(t.type==INT && u.word=="NUM"){
//                 tree.top()->children.push_back(new SynNode{to_string(t.i_value), INT});
//             }else if(t.s_value==u.word || t.type==ID && u.word=="ID"){
//                 tree.top()->children.push_back(new SynNode{u.word, t.type});
//             }else{
//                 cout << "match failed with token:" <<t.s_value<<", expected:"<<u.word<<endl; 
//                 throw runtime_error("error!");
//             }
//         }else{//栈顶非终结符
//             S.pop();
//             if(t.type==ID || t.type==INT){
//                 if (table[u.word].count("NUM")==0 && table[u.word].count("ID")==0)
//                     throw runtime_error("product doesn't exists!");
//                 else{
//                     if(t.type==ID)
//                         string type("ID");
//                     else
//                         string type("NUM");
//                     int idx = table[u.word][type];
//                     for(ProUnit tp:m_grammar[u.word][idx]){
//                         S.push(tp);
//                         tree.top()->children.push_back(new SynNode{tp.word});
//                     }
//                 }
//             }else{
//                 if (table[u.word].count(t.s_value) == 0)
//                     throw runtime_error("product doesn't exists!");
//                 else{
//                     int idx = table[u.word][t.s_value];
//                     for (ProUnit tp : m_grammar[u.word][idx])
//                     {
//                         S.push(tp);
//                     }
//                 }
//             }
//             lex.toMarker(mk);
//         }
//     }
// }
#endif