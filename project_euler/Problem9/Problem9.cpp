#include <iostream>
using namespace std;

int main() {

	for (long long int a = 0; a < 1000; a++) {
		for (long long int b = 0; b < 1000; b++) {
			for (long long int c = 0; c < 1000; c++) {
				if (a + b + c == 1000) {
					if ((a*a + b*b) == (c*c)) {
						cout << a << " " << b << " " << c << endl;
						cout << a * b*c << endl;
					}
				}
			}
		}
	}
	return 0;
}