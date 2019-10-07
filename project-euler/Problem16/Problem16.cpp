#include<iostream>
using namespace std;

long long unsigned int ourNum[350];

void multiplyby2() {
	//multiply
	for (int i = 0; i < 350; i++) {
		ourNum[i] = ourNum[i] * 2;
	}
	//carry remainders
	for (int i = 0; i < 350; i++) {
		if (ourNum[i] >= 10) {
		ourNum[i] = ourNum[i] % 10;
		ourNum[i + 1]++;
		}
	}
}

int main() {
	//zero out ourNum
	for (int i = 0; i < 350; i++) {
		ourNum[i] = 0;
	}
	//Set to multiplicative constant
	ourNum[0] = 1;
	
	for (int i = 0; i < 1000; i++) {
		multiplyby2();
		for (int i = 0; i < 350; i++) {
			cout << ourNum[i];
		}
		cout << endl;
	}

	long long unsigned int currSum = 0;

	for (int i = 0; i < 350; i++) {
		currSum += ourNum[i];
	}

	cout << currSum << endl;

	return 0;
}