#include <bits/stdc++.h>
#include <fstream>
using namespace std;
int main()
{
    srand(time(NULL));
    string s;
    s="PATH/FILE.bin";
    ofstream fo;
    fo.open(s,ios::out|ios::binary|ios::app);
    int n;
    cin>>n;
    double a,b,c;
    for (int i=0;i<n;i++)
    {
        a=rand()%10000 + 1;
        b=rand()%10000 + 1;
        c=double(double(a)/double(b));
        fo.write(reinterpret_cast<char *>(&c), 8);
    }
    fo.close();
    return 0;
}
