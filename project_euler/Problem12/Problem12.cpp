#include<iostream>
using namespace std;

int main() {

	long long unsigned int trianglenum = 1;
	long long unsigned int pos = 2;
	int divisors = 0;
	int pos2=0;
	while(divisors<=500) {
		divisors = 1;
		trianglenum+=pos;
		pos++;
		pos2=0;
		int array[501];
		for(int i = 0;i <501;i++) {
			array[i]=0;
		}
		long long unsigned int current = trianglenum;
		for(long long unsigned int i = 2;i<=current;i++) {	
			while(current%i==0) {
				array[pos2]++;
                                current=current/i;
			}
			if(array[pos2]!=0) pos2++;
		}
		for(long long unsigned int i = 0;i <501;i++) {
			divisors=divisors*(array[i]+1);
		}
		cout << trianglenum << " " << divisors<< endl;
	}
	cout << trianglenum << endl;
	return 0;

}
