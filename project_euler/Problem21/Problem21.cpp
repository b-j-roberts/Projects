#include <iostream>

long long unsigned int pdSum(long long unsigned int num) {
	long long unsigned int sum = 0;
	for (long long unsigned int i = 1; i < num; i++) {
		if (num%i == 0) sum += i;
	}
	return sum;
}

bool isAmicable(long long unsigned int num) {
	long long unsigned int b;
	b = pdSum(num);
	if (pdSum(b) == num && b!=num) return true;
	return false;
}

class boolHold {
private:
	bool boolean;
	bool Eval;
public:
	boolHold();
	void set(bool);
	bool check();
	bool get();
};

boolHold::boolHold() {
	Eval = false;
}

void boolHold::set(bool boolean) {
	Eval = true;
	this->boolean = boolean;
}

bool boolHold::check() {
	return Eval;
}

bool boolHold::get() {
	if (check()) return boolean;
	else {
		std::cout << "Error, trying to get unset boolHold" << std::endl;
		exit(0);
	}
}

int main() {

	boolHold array[10000];
	for (long long unsigned int i = 1; i < 10000; i++) {
		if (!array[i].check()) {
			if (isAmicable(i)) {
				array[i].set(true);
				long long unsigned int b = pdSum(i);
				if (b < 10000)array[b].set(true);
			}
			else array[i].set(false);
		}
	}
	long long unsigned int sum = 0;
	for (int i = 1; i < 10000; i++) {
		if (array[i].get()) sum += i;
	}

	std::cout << sum << std::endl;

	return 0;
}