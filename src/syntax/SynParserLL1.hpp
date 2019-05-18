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
#include "../lexical/LexParser.hpp"
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
    set<ProUnit> getFIRSThelper(vector<ProUnit>::iterator begin,vector<ProUnit>::iterator end, int from);
    void loadGrammar();
    void getFIRST();
    void getFOLLOW();
    void genParseTable(); //生成预测分析表
    void parse();
    void debug();
};

SynParser::SynParser(LexParser &lex_) : lex(lex_)
{
    cout << "parsing..." << endl;
    loadGrammar();
    getFIRST();
    getFOLLOW();
    genParseTable();
    parse();
    // debug();
}

void SynParser::debug()
{ 
    //输出分析表
    for(auto it=table.begin();it!=table.end();++it){
        cout << it->first<<endl;
        for(auto at=it->second.begin();at != it->second.end();++at){
            cout << at->first<<' '<<at->second<<"   ";
        }
        cout << endl;
    }
    
    // //输出first follow语法结果
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
    f_grm.open("./syntax/grammarLL1.txt");
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

void SynParser::getFIRST()
{
    //规则1，每条产生式首个终结符直接加入first
    bool more = true;
    while (more)
    {
        more = false;
        for (int i=0; i<m_grammar.size(); ++i)
        {   
            auto has_new = FIRST[m_grammar[i][0].word].size(); //初始大小
            auto ret = getFIRSThelper(m_grammar[i].begin()+1, m_grammar[i].end(), i);
            FIRST[m_grammar[i][0].word].insert(ret.begin(), ret.end());
            more |= (has_new!=FIRST[m_grammar[i][0].word].size());
        }
    }
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

void SynParser::parse(){
    stack<ProUnit> S; //符号栈
    S.push(ProUnit{"$"});
    S.push(ProUnit{"program"});
    ProUnit u = S.top();  //栈顶符号
    while(u.word != "$"){
        auto mk = lex.getMarker();
        Token tk = lex.nextToken();
        if(u.isTerminal()){
            if((u.word=="NUM" && tk.type==INT) || (u.word=="ID" && tk.type==ID) || (u.word==tk.s_value)){
                cout << tk.strfToken() << endl;
                S.pop();
            }else{
                cout << "tk - u:(" << TYPE_NAME[tk.type]<<")"<<tk.s_value<<" - "<<u.word<<endl;
                throw runtime_error("terminal match failed!");
            }
        }else{//非终结符
            lex.toMarker(mk);
            int idx = -1;
            if(tk.type==INT)
                idx = table[u.word]["NUM"];
            else if(tk.type==ID)
                idx = table[u.word]["ID"];
            else
                idx = table[u.word][tk.s_value];
            vector<ProUnit> pd = m_grammar[idx];

            //输出产生式和栈操作
            S.pop();
            for(auto it=pd.begin(); it!=pd.end();++it){
                cout << it->word << ' ';
            }
            for(auto it=pd.rbegin(); it!=pd.rend()-1;++it){//产生式末尾先入栈
                if(it->word=="empty")
                    continue;
                S.push(*it);
            }
            cout << endl;
        }
        u = S.top();
    }
    cout << "#####"<<endl;
}
#endif