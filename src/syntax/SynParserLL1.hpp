#ifndef SYN_PARSER_LL1_H
#define SYN_PARSER_LL1_H
#include <fstream>
#include <stack>
#include "../lexical/LexParser.hpp"
#include "../utils.hpp"
using namespace std;

extern map<string, set<ProUnit>> getFIRST(vector<vector<ProUnit>>);
extern map<string, set<ProUnit>> getFOLLOW(vector<vector<ProUnit>>, map<string, set<ProUnit>>);

class SynParser
{
private:
    LexParser &lex;
    vector<vector<ProUnit>> m_grammar;
    map<string, set<ProUnit>> FIRST, FOLLOW;
    map<string, map<string, int>> table; //非终结符-终结符-对应非终结符在grammar中的产生式下标

public:
    SynParser(LexParser &lex_); //读取产生式

private:
    void loadGrammar();
    void genParseTable(); //生成预测分析表
    void parse();
    void debug();
};

SynParser::SynParser(LexParser &lex_) : lex(lex_)
{
    cout << "parsing..." << endl;
    loadGrammar();
    FIRST = getFIRST(m_grammar);
    FOLLOW = getFOLLOW(m_grammar, FIRST);
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
    f_grm.open("./syntax/grammarLL1.txt");
    if(!f_grm.is_open()){
        throw runtime_error("error while open grammar.txt");
    }
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
void SynParser::genParseTable()
{
    for(auto it=FIRST.begin(); it!=FIRST.end(); ++it){
        //搜索FIRST集，填写预测分析表
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
    S.push(ProUnit{"declaration-list"});
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
            if(idx==0){cout<<"error:" << tk.strfToken()<<endl; throw runtime_error("error");} 
            vector<ProUnit> pd = m_grammar[idx];

            //输出产生式和栈的维护
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
    cout << "#####"<<endl; //分析完成
}
#endif