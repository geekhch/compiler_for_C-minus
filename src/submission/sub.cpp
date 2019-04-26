#include<iostream>
#include<vector>
#include<set>
#include<string>
#include<map>
#include<cstring>
using namespace std;

struct Production{//产生式
    char left; //左部
    string right; //右部
};

bool getFirstByString(char, string, map<char,set<char>>&); //利用规则二对一个字符串求first
void getFirst(vector<Production>&, map<char,set<char>>&); //first总函数
void getFollow(vector<Production>&, map<char,set<char>>& first, map<char,set<char>>& follow);
void print(string, map<char,set<char>>);

int main()
{
    map<char,set<char>> FIRST,FOLLOW;
    char left;string right;
    vector<Production> prods; //产生式

    while(cin>>left>>right && (left!='0' &&right!="0")){//输入并初始化first、follow集合
        if(!FIRST.count(left)) FIRST[left] = set<char>{};
        if(!FOLLOW.count(left)) FOLLOW[left] = set<char>{};
        prods.push_back(Production{left, right});
    }

    getFirst(prods, FIRST);
    getFollow(prods, FIRST, FOLLOW);
    print("FIRST",FIRST);
    print("FOLLOW",FOLLOW);
    return 0;
}


bool getFirstByString(char left, string s, map<char,set<char>> &FIRST){
    int has_new = FIRST[left].size();
    for(auto c: s){//如果前面的非终结符推导出空则继续往后推导
        if(isupper(c)) {//终结符或空
            for(char c:FIRST[c]){
                FIRST[left].insert(c);
            }
            if(FIRST[c].count('^')==0) break; //不含空
        }else{
            FIRST[left].insert(c);
            break;
        }
    }
    return (has_new != FIRST[left].size()); //有新加入元素
}

void getFirst(vector<Production> &prods, map<char,set<char>> &FIRST){
    for(auto &p: prods){//规则1
        if(!isupper(p.right[0]) && FIRST[p.left].count(p.right[0])==0) {//终结符或空
            FIRST[p.left].insert(p.right[0]);
        }
    }
    bool flag = true;
    while(flag){//规则2
        flag = false;
        for(auto &p: prods){
            flag = flag|getFirstByString(p.left, p.right, FIRST);
        }
    }
}

void getFollow(vector<Production>& prods, map<char,set<char>>& FIRST, map<char,set<char>>& FOLLOW){
    //规则1
    FOLLOW[prods[0].left].insert('$');
    //规则2，终结符后面的first集加入到当前follow
    bool has_more = true;
    map<char,int> CC;
    while(has_more){
        has_more = false;
        //统计当前集合元素数目
        for(auto it:FOLLOW){
            CC[it.first] = it.second.size();
        }
        for(auto p:prods){
            string &s = p.right;
            for(int i=0; i<s.size()-1;i++){
                if(isupper(s[i])){//出现非终结符
                    for(int j=i+1;j<s.size();j++){
                        if(!isupper(s[j])){
                            FOLLOW[s[i]].insert(s[j]);break;
                        }else{
                            FOLLOW[s[i]].insert(FIRST[s[j]].begin(),FIRST[s[j]].end());
                            if(FIRST[s[j]].count('^')==0) break;
                        }
                    }
                }
            }
            for(int i=s.size()-1;i>=0;i--){
                if(isupper(s[i])){
                    FOLLOW[s[i]].insert(FOLLOW[p.left].begin(),FOLLOW[p.left].end());
                    if(FIRST[s[i]].count('^')>0) 
                        continue;
                }
                break;
            }
        }
        for(auto it:FOLLOW){
            has_more = has_more|(CC[it.first] != it.second.size());
        }
    }
    for(auto &it:FOLLOW){
        it.second.erase('^');
    }
}

void print(string type, map<char,set<char>> m){
    for(auto it:m){
        cout << type<<'('<<it.first << "): ";
        for(auto c:it.second){
            cout << c << ' ';
        }
        cout << endl;
    }
}
