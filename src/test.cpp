#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <set>
#include <algorithm>
#include <queue>
using namespace std;


int main()
{
    vector <vector<int>> r;
    vector<int> rr{1,2,3,4,5,6};
    r.push_back(rr);
    rr.clear();
    rr.push_back(9);
    r.push_back(rr);
    for(auto p:r){
        for(auto o:p){
            cout << o<<' ';
        }
        cout << endl;
    }
    return 0;
}
