#include<iostream>
using namespace std;

int f(int a){
    return 2*a;
    a=9;
}

int g(int a, int b){
    int x;
    x=f(a)+a+b;
    return x;
}

int main(){
    int a,b,c[3];
    a=1*(2+3)%3;
    b= 1<5;
    c[0]=2;
    if(a && b)
        c[0]++;
    else
        c[1]=c[0];
    cout<<a<<" "<<b<<endl;
}


