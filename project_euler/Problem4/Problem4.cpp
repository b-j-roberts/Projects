#include<iostream>
using namespace std;

int getLength(long int number) {
  int length = 0;
  while(number!=0) {
    length++;
    number = number/10;
  }
  return length;
}

bool isPalindrome(long int number) {
  int length = getLength(number);
  int digits[length];
  int pos = 0;
  while(number!=0) {
    digits[pos]=number%10;
    number=number/10;
    pos++;
  }
  pos = 0;
  bool Palindrome = true;
  while(Palindrome && pos<length) {
    if(digits[pos]!=digits[length-pos-1]) Palindrome=false;
    pos++;
  } 
  return Palindrome;
}

int main() {

long int number = 1;
long int test;
for(long int i=999;i>99;i--) {
  for(long int j=i;j<1000;j++) {
    test=j*i;
    cout << i << "*" << j << "=" << test << endl;
    if(isPalindrome(test) && test > number) number=test; 
  }
}

cout << number << endl;

return 0;
}
