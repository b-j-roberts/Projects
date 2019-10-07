#include <iostream>
using namespace std;

bool isSunday(long long unsigned int day) {

	if (day % 7 == 0) return true;
	else return false;
}

void addJanuary(long long unsigned int &day) {
	day += 31;
}

void addFebuary(long long unsigned int &day, int year) {
	day += 28;
	if (year % 4 == 0) {
		day += 1;
		if (year % 100 == 0) {
			day -= 1;
			if (year % 400 == 0) {
				day += 1;
			}
		}
	}
}

void addMarch(long long unsigned int &day) {
	day += 31;
}

void addApril(long long unsigned int &day) {
	day += 30;
}

void addMay(long long unsigned int &day) {
	day += 31;
}

void addJune(long long unsigned int &day) {
	day += 30;
}

void addJuly(long long unsigned int &day) {
	day += 31;
}

void addAugust(long long unsigned int &day) {
	day += 31;
}

void addSeptember(long long unsigned int &day) {
	day += 30;
}

void addOctober(long long unsigned int &day) {
	day += 31;
}

void addNovember(long long unsigned int &day) {
	day += 30;
}

void addDecember(long long unsigned int &day) {
	day += 31;
}

int main() {

	int year = 1901;
	long long unsigned int day = 2;
	long long unsigned int numofSun = 0;
	while (year < 2001) {
		addJanuary(day);
		if (isSunday(day)) numofSun++;
		addFebuary(day,year);
		if (isSunday(day)) numofSun++;
		addMarch(day);
		if (isSunday(day)) numofSun++;
		addApril(day);
		if (isSunday(day)) numofSun++;
		addMay(day);
		if (isSunday(day)) numofSun++;
		addJune(day);
		if (isSunday(day)) numofSun++;
		addJuly(day);
		if (isSunday(day)) numofSun++;
		addAugust(day);
		if (isSunday(day)) numofSun++;
		addSeptember(day);
		if (isSunday(day)) numofSun++;
		addOctober(day);
		if (isSunday(day)) numofSun++;
		addNovember(day);
		if (isSunday(day)) numofSun++;
		addDecember(day);
		year++;
		if (isSunday(day)) numofSun++;
	}

	cout << numofSun << endl;

	return 0;
}