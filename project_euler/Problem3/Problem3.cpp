#include<iostream>
using namespace std;

bool isDivisible(long long int number,long long int divisor) {
if(number%divisor==0) return true;
else return false;
}

bool isPrime(long long int i) {
  bool Prime=true;
  long long int count = 2;
  while(Prime && count < i/2) {
    Prime = 1-isDivisible(i,count);
    count++;
    }
  if(count >= i/2) return true;
  return false;
}

int main() {

long long int number = 600851475143;
long long int maxprimeDiv = 1;
for(long long int i=2;i<=number;i++) {
  if(isPrime(i)) {
    cout << "Testing prime " << i << endl;
    if(isDivisible(number,i)) {
      maxprimeDiv = i;
      while(isDivisible(number,i)) number=number/i;
      cout << "Divisible by " << i << endl;
    }
  }
}
cout<< maxprimeDiv << endl;
return 0;
}
