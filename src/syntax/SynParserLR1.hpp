#ifndef SYN_PARSER_LR1_H
#define SYN_PARSER_LR1_H
#include <fstream>
#include <stack>
#include "../lexical/LexParser.hpp"
#include "../utils.hpp"
using namespace std;

extern map<string, set<ProUnit>> getFIRST(vector<vector<ProUnit>>);
extern map<string, set<ProUnit>> getFOLLOW(vector<vector<ProUnit>>, map<string, set<ProUnit>>);
extern set<Item> closure(vector<vector<ProUnit>>, Item);

class SynParser
{
private:
    LexParser &lex;
    vector<vector<ProUnit>> m_grammar;
    map<string, set<ProUnit>> FIRST, FOLLOW;

public:
    SynParser(LexParser &lex_); //读取产生式
private:
    void loadGrammar();
    void debug();
};

SynParser::SynParser(LexParser &lex_) : lex(lex_)
{
    cout << "parsing..." << endl;
    loadGrammar();
    FIRST = getFIRST(m_grammar);
    FOLLOW = getFOLLOW(m_grammar, FIRST);
    debug();
}

void SynParser::debug()
{ 
    cout <<"here are "<< m_grammar.size() <<" producitons in grammar"<<endl;

    // set<Item> test= closure(m_grammar,Item{0,0});
    // for(auto it = test.begin(); it!=test.end();++it){
    //     cout << it->pd_idx << ' '<< it->dot_idx<<endl;
    // }
    //输出分析表
    // for(auto it=table.begin();it!=table.end();++it){
    //     cout << it->first<<endl;
    //     for(auto at=it->second.begin();at != it->second.end();++at){
    //         cout << at->first<<' '<<at->second<<"   ";
    //     }
    //     cout << endl;
    // }
    
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
    f_grm.close();
}

#endif
