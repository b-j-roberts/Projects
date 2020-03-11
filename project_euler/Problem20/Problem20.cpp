#include <iostream>

//Note: could do this recursively if nums larger than 99 needed
void multiply(int *number, int mult) {
	int ones[300];
	for (int i = 0; i < 300; i++) {
		ones[i] = 0;
	}
	ones[0] = number[0] * (mult % 10);
	for (int i = 1; i < 300; i++) {
		ones[i] = number[i] * (mult % 10) + (ones[i - 1] / 10);
		ones[i - 1] = ones[i - 1] % 10;
	}
	mult = mult / 10;
	int tens[300];
	for (int i = 0; i < 300; i++) {
		tens[i] = 0;
	}
	if (mult != 0) {
		for (int i = 0; i < 299; i++) {
			tens[i + 1] = (number[i] * mult) + (tens[i] / 10);
			tens[i] = tens[i] % 10;
		}
	}
	number[0] = ones[0];
	for (int i = 1; i < 300; i++) {
		number[i] = tens[i] + ones[i] + (number[i-1] / 10);
		number[i - 1] = number[i - 1] % 10;
	}
}

int main() {

	//100! is definitely less than 300 digits because 100^100 has around 200 0s and 100!<<100^100.
	int number[300];
	//Zero out
	for (int i = 1; i < 300; i++) {
		number[i] = 0;
	}
	//Multiplicative identity
	number[0] = 1;

	//multiply up to 100 (*100 only adds 2 zeros so we only need to worry about up to 99 for sum)
	for (int i = 2; i < 100; i++) {
		for (int j = 0; j < 300; j++) {
			std::cout << number[j];
		}
		std::cout << std::endl;
		multiply(number, i);
	}
	int sum = 0;
	for (int i = 0; i < 300; i++) {
		sum += number[i];
	}
	std::cout << sum << std::endl;

	return 0;
}