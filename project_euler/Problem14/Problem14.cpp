#include <iostream>
using namespace std;

long long unsigned int CollatzAlg(long long unsigned int num) {
	long long unsigned int temp;
	if (num % 2 == 0) temp = num / 2;
	else			  temp = 3 * num + 1;
	return temp;
}

int main() {

	int count, maxcount=1;
	for (long long unsigned int i = 2; i < 1000000; i++) {
		long long unsigned int hold = i;
		count = 1;
		while (hold != 1) {
			hold = CollatzAlg(hold);
			count++;
		}
		if (count > maxcount) {
			maxcount = count;
			cout << maxcount << " " << i << endl;
		}
	}

	return 0;
}