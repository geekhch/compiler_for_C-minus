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
#include "LexParser.hpp"
using namespace std;

struct SynNode //语法树结点
{
    string word; //结点内容
    int type;
    vector<SynNode *> children; //子节点(为空时表示根节点，即终结符)
    // SynNode(string s):word(s){}
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
    map<string, vector<vector<ProUnit>>> m_grammar;
    map<string, set<ProUnit>> FIRST, FOLLOW;
    map<string, map<string, int>> table; //非终结符-终结符-对应非终结符在grammar中的产生式下标

public:
    SynParser(LexParser &lex_); //读取产生式
private:
    bool getFIRSThelper(string left, vector<ProUnit> &pd, int);
    void loadGrammar();
    void getFIRST();
    void getFOLLOW();
    void genParseTable(); //生成预测分析表
};

SynParser::SynParser(LexParser &lex_) : lex(lex_)
{
    loadGrammar();
    getFIRST();
    getFOLLOW();
    genParseTable();
    //输出分析表
    // for(auto it=table.begin();it!=table.end();++it){
    //     cout << it->first<<endl;
    //     for(auto at=it->second.begin();at != it->second.end();++at){
    //         cout << at->first<<' '<<at->second<<"   ";
    //     }
    //     cout << endl;
    // }
    
    //输出first follow语法结果
    // cout <<"first:"<<endl;
    // for(auto it=FIRST.begin();it!=FIRST.end();++it){
    //     cout <<it->first<<": ";
    //     for(auto u:it->second){
    //         cout <<u.word<<u.from<<" ";
    //     }
    //     cout << endl;
    // }
    // cout << endl <<"follow:"<<endl;
    // for(auto it=FOLLOW.begin();it!=FOLLOW.end();++it){
    //     cout <<it->first<<": ";
    //     for(auto u:it->second){
    //         cout <<u.word<<" ";
    //     }
    //     cout << endl;
    // }
}

void SynParser::loadGrammar()
{ //将文法产生式保存为map-vector数据结构
    fstream f_grm;
    f_grm.open("./syntax/grammar.txt");
    string word;
    string noTerminal;
    f_grm >> noTerminal;
    vector<ProUnit> curPro;
    while (f_grm >> word)
    {
        curPro.push_back(ProUnit{word});

        if (f_grm.get() == '\n')
        { //一行结束，将当前符号加入对应非终结符的产生式
            m_grammar[noTerminal].push_back(curPro);
            curPro.clear();
            f_grm >> noTerminal;
        }
    }
}

bool SynParser::getFIRSThelper(string left, vector<ProUnit> &pd, int from)
{//对一条产生式求first集
    auto has_new = FIRST[left].size();
    for (auto it = pd.begin(); it != pd.end(); it++)
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
        for (auto it = m_grammar.begin(); it != m_grammar.end(); it++)
        {
            int i=0;
            for (auto pd : it->second)
            {
                more |= getFIRSThelper(it->first, pd, i);
                i++;
            }
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
        for (auto it = m_grammar.begin(); it != m_grammar.end(); ++it)
        {
            for (auto pd : it->second)
            {
                //规则2 对一个产生式中的所有非终结符，将后面的first集加入其follow集
                for (int i = 0; i < pd.size() - 1; ++i)
                {
                    if (!pd[i].isTerminal())
                    { // 找到一个非终结符pd[i]
                        auto has_more = FOLLOW[pd[i].word].size();
                        for (int j = i + 1; j < pd.size(); ++j)
                        {
                            if (pd[j].isTerminal())
                                FOLLOW[pd[i].word].insert(pd[j]);
                            else
                            {
                                FOLLOW[pd[i].word].insert(FIRST[pd[j].word].begin(), FIRST[pd[j].word].end());
                                if (FIRST[pd[j].word].count(ProUnit{"empty"}) == 0)
                                    break;
                            }
                        }
                        more |= (has_more != FOLLOW[pd[i].word].size());
                    }
                }

                //规则3 以非终结符结尾的，将左部的follow集并入末尾非终结符
                for (int j = pd.size() - 1; j >= 0; j--)
                {
                    if (pd[j].isTerminal())
                        break;

                    auto has_more = FOLLOW[pd[j].word].size();
                    FOLLOW[pd[j].word].insert(FOLLOW[it->first].begin(), FOLLOW[it->first].end());
                    more |= (has_more != FOLLOW[pd[j].word].size());

                    if (FIRST[pd[j].word].count(ProUnit{"empty"}) == 0)
                        break; //不含空集，不再往前推导
                }
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
#endif