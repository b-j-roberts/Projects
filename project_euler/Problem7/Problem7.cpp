#include <iostream>
#include <math.h>
using namespace std;

bool isPrime(long long int number) {

	for(long long int i=2;i<=sqrt(number);i++) {
		if(number%i==0) return false;
	}
	return true;
}

int main() {
	int count = 0;
	long long int number = 2;
	while(count < 10001) {
		if(isPrime(number)) count ++;
		number++;
	}
	
	cout << number << endl;
	int hold;
	cin >> hold;
	return 0;
}
