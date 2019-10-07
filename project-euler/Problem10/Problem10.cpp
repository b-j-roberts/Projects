#include <iostream>
#include <math.h>
using namespace std;

bool isPrime(long long unsigned int number) {

	for(long long unsigned int i=2;i<=sqrt(number);i++) {
		if(number%i==0) return false;
	}
	return true;
}

int main() {

	long long unsigned int sum = 0;
	for (long long unsigned int prime = 2; prime < 2000000; prime++) {
		if (isPrime(prime)) sum += prime;
	}
	cout << sum << endl;
	return 0;
}