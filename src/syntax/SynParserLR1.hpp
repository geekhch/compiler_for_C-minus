#ifndef SYN_PARSER_LR1_H
#define SYN_PARSER_LR1_H
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <fstream>
#include <map>
#include <set>
#include <fstream>
#include <stack>
#include "../lexical/LexParser.hpp"
using namespace std;

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
    LexParser &lex;
    vector<vector<ProUnit>> m_grammar;
    map<string, set<ProUnit>> FOLLOW;

public:
    SynParser(LexParser &lex_); //读取产生式
private:
    set<ProUnit> getFIRSThelper(vector<ProUnit>::iterator begin,vector<ProUnit>::iterator end, int from);
    void loadGrammar();
    void getFOLLOW();
};

SynParser::SynParser(LexParser &lex_) : lex(lex_)
{
    cout << "parsing..." << endl;
    loadGrammar();
    getFOLLOW();

    // //输出first follow语法结果
    // cout <<"first:"<<endl;
    // for(auto it=FIRST.begin();it!=FIRST.end();++it){
    //     cout <<it->first<<": ";
    //     for(auto u:it->second){
    //         cout <<u.word<<u.from<<" ";
    //     }
    //     cout << endl;
    // }
    // cout << endl <<"follow:"<<endl;
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
    f_grm.open("./syntax/grammarLR1.txt");
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

set<ProUnit> SynParser::getFIRSThelper(vector<ProUnit>::iterator begin,vector<ProUnit>::iterator end,int from)
{//对一条产生式序列求first集
    set<ProUnit> ret;
    for (auto it = begin; it != end; it++)
    {
        if (!it->isTerminal())
        {//非终结符
            for(auto u: FIRST[it->word]){
                u.from = from;
                if(u.word!="empty" || it==end-1) //如果产生式推导出空
                    ret.insert(u);
            }
            //当前非终结符不能推出空停止扫描
            if (FIRST[it->word].count(ProUnit{"empty"}) == 0)
                break;
        }
        else
        { //遇到终结符停止扫描
            it->from = from;
            ret.insert(*it);
            break;
        }
    }
    return ret;
}

void SynParser::getFOLLOW()
{
    //规则1：开始符号加入$
    FOLLOW["program"].insert(ProUnit{"$"});

    bool more = true;
    while (more)
    {
        more = false;
        for (int idx=0; idx< m_grammar.size(); ++idx)
        {   
            auto pd = m_grammar[idx];

            //规则2 对一个产生式中的所有非终结符，将后面的first集加入其follow集
            for (int i = 1; i < pd.size() - 1; ++i)
            {//从前往后
                if (!pd[i].isTerminal())
                { // 找到一个非终结符pd[i],将后面的first集并入
                    string noTerminal = pd[i].word;
                    auto hasMore = FOLLOW[noTerminal].size();
                    auto ret = getFIRSThelper(pd.begin()+i+1,pd.end(),idx); //idx记录对应产生式
                    if(ret.count(ProUnit{"empty"})>0)
                        ret.erase(ProUnit{"empty"});
                    FOLLOW[noTerminal].insert(ret.begin(),ret.end());
                    more |= (hasMore != FOLLOW[noTerminal].size());
                }
            }

            //规则3 以非终结符结尾的，将左部的follow集并入末尾非终结符
            for (int j = pd.size() - 1; j > 0; j--)
            {//从后往前
                if (pd[j].isTerminal())
                    break;
                auto hasMore = FOLLOW[pd[j].word].size();
                for(auto it=FOLLOW[pd[0].word].begin(); it != FOLLOW[pd[0].word].end(); ++it){
                    FOLLOW[pd[j].word].insert(ProUnit{it->word, idx}); //记录对应产生式
                }
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

#endif