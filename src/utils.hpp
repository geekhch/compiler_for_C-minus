/**
 * this file define the auxiliary classes and functions, including:
 * 1. FIRST and FOLLOW sets caculation.
 * 2. parsing tree Node .
 * 3. element class of productions
*/
#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include "lexical/LexParser.hpp"
using namespace std;


struct SynNode //语法树结点
{
    string word;                //结点内容
    int type = -1;              //用于输出语法树时标注终结符词素类型（id key op...）
    vector<SynNode *> children; //子节点(为空时表示根节点，即终结符)
};

struct ProUnit
{ //一个终结符或非终结符
    string word;
    int from = -1;
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

struct Item{
    int pd_idx;
    int dot_idx;

    bool operator<(const Item o) const
    {
        if(pd_idx<o.pd_idx) return true;
        if(pd_idx==o.pd_idx && dot_idx<o.dot_idx) return true;
        return false;
    }
};


/*************   类型定义   ****************/
typedef set<Item> Closure;
typedef vector<ProUnit> Production; //产生式
/****************************************************/

set<ProUnit> getFIRSThelper(map<string, set<ProUnit>> &FIRST, Production::iterator begin, Production::iterator end, int from)
{ //对一条产生式序列求first集
    set<ProUnit> ret;
    for (auto it = begin; it != end; it++)
    {
        if (!it->isTerminal())
        { //非终结符
            for (auto u : FIRST[it->word])
            {
                u.from = from;
                if (u.word != "empty" || it == end - 1) //如果产生式推导出空
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

map<string, set<ProUnit>> getFIRST(vector<Production> m_grammar)
{
    map<string, set<ProUnit>> FIRST;
    //规则1，每条产生式首个终结符直接加入first
    bool more = true;
    while (more)
    {
        more = false;
        for (int i = 0; i < m_grammar.size(); ++i)
        {
            auto has_new = FIRST[m_grammar[i][0].word].size(); //初始大小
            auto ret = getFIRSThelper(FIRST, m_grammar[i].begin() + 1, m_grammar[i].end(), i);
            FIRST[m_grammar[i][0].word].insert(ret.begin(), ret.end());
            more |= (has_new != FIRST[m_grammar[i][0].word].size());
        }
    }
    return FIRST;
}

map<string, set<ProUnit>> getFOLLOW(vector<Production> m_grammar, map<string, set<ProUnit>> FIRST)
{
    map<string, set<ProUnit>> FOLLOW;

    //规则1：开始符号加入$
    FOLLOW["program"].insert(ProUnit{"$"});

    bool more = true;
    while (more)
    {
        more = false;
        for (int idx = 0; idx < m_grammar.size(); ++idx)
        {
            auto pd = m_grammar[idx];

            //规则2 对一个产生式中的所有非终结符，将后面的first集加入其follow集
            for (int i = 1; i < pd.size() - 1; ++i)
            { //从前往后
                if (!pd[i].isTerminal())
                { // 找到一个非终结符pd[i],将后面的first集并入
                    string noTerminal = pd[i].word;
                    auto hasMore = FOLLOW[noTerminal].size();
                    auto ret = getFIRSThelper(FIRST, pd.begin() + i + 1, pd.end(), idx); //idx记录对应产生式
                    if (ret.count(ProUnit{"empty"}) > 0)
                        ret.erase(ProUnit{"empty"});
                    FOLLOW[noTerminal].insert(ret.begin(), ret.end());
                    more |= (hasMore != FOLLOW[noTerminal].size());
                }
            }

            //规则3 以非终结符结尾的，将左部的follow集并入末尾非终结符
            for (int j = pd.size() - 1; j > 0; j--)
            { //从后往前
                if (pd[j].isTerminal())
                    break;
                auto hasMore = FOLLOW[pd[j].word].size();
                for (auto it = FOLLOW[pd[0].word].begin(); it != FOLLOW[pd[0].word].end(); ++it)
                {
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

    return FOLLOW;
}


Closure closure(vector<Production> m_grammar,Item item){
    Closure CLOSURE;
    CLOSURE.insert(item);
    
    while(true){
        Closure tmp; //本次迭代新增的项
        for(auto it=CLOSURE.begin(); it!=CLOSURE.end();it++){
            auto pd = m_grammar[it->pd_idx];

            if(it->dot_idx < (pd.size()-1) && !pd[it->dot_idx+1].isTerminal()){
                //所有该终结符为左部的产生式加入进来
                for(int i=1;i<m_grammar.size(); ++i){
                    if(m_grammar[i][0] == pd[it->dot_idx+1] && CLOSURE.count(Item{i,0})==0)
                        tmp.insert(Item{i,0});
                }
            }
        }
        if(tmp.empty()) break;
        CLOSURE.insert(tmp.begin(),tmp.end());
    }
    return CLOSURE;
}


#endif