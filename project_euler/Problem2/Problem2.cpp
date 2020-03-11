#include<iostream>
using namespace std;

int main(){
int Fibevensum=2;
int F2=2;
int F1=1;
int F3=0;
int F4=0;
while(F2<4000000) {
F3=F2+F1;
F4=F3+F2;
Fibevensum+=F3+F4;
F1=F4;
F2=F4+F3;
}
Fibevensum-=F2;
cout << Fibevensum <<endl;
return 0;
}
