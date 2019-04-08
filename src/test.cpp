#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
using namespace std;


class LexPaser
{
private:
    vector<string> type = {"hello","yes"};
};

int main()
{
    char s[50];
    string nstr("hello");
    sprintf(s,"<%20s>",nstr.c_str());
    sprintf(s,"<%20s>","gg");
    cout << s << endl;
    return 0;
}
