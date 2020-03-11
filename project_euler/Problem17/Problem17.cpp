#include <iostream>

using namespace std;

long long unsigned int lettersUsed(long long unsigned int number) {
	long long unsigned int total = 0;
	if (number >= 100) {
		total += lettersUsed(number / 100); //adds hundreds place digit sum
		total += 7; //adds 7 for letters in hundred
		long long unsigned int numHundreds = number / 100;
		number = number - 100 * numHundreds;
		if (number != 0) total += 3; //adds 3 for letters in and when that happens
	}
	if (number >= 20) {
		if (number / 10 == 2) total += 6;
		if (number / 10 == 3) total += 6;
		if (number / 10 == 4) total += 5;
		if (number / 10 == 5) total += 5;
		if (number / 10 == 6) total += 5;
		if (number / 10 == 7) total += 7;
		if (number / 10 == 8) total += 6;
		if (number / 10 == 9) total += 6;
		total += lettersUsed(number - 10 * (number / 10));
	}
	if (number == 1) total += 3;
	if (number == 2) total += 3;
	if (number == 3) total += 5;
	if (number == 4) total += 4;
	if (number == 5) total += 4;
	if (number == 6) total += 3;
	if (number == 7) total += 5;
	if (number == 8) total += 5;
	if (number == 9) total += 4;
	if (number == 10) total += 3;
	if (number == 11) total += 6;
	if (number == 12) total += 6;
	if (number == 13) total += 8;
	if (number == 14) total += 8;
	if (number == 15) total += 7;
	if (number == 16) total += 7;
	if (number == 17) total += 9;
	if (number == 18) total += 8;
	if (number == 19) total += 8;
	return total;
}

int main() {

	long long unsigned int sum = 0;
	for (int i = 1; i < 1000; i++) {
		sum += lettersUsed(i);
	}
	sum += 11;//one thousand
	cout << sum << endl;

	return 0;
}