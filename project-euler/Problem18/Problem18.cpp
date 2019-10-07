#include<iostream>
#include<string>
using namespace std;

long long unsigned int triang[15][15] = {
		{75},
		{95,64},
		{17,47,82},
		{18,35,87,10},
		{20,4,82,47,65},
		{19,1,23,75,3,34},
		{88,2,77,73,7,63,67},
		{99,65,4,28,6,16,70,92},
		{41,41,26,56,83,40,80,70,33},
		{41,48,72,33,47,32,37,16,94,29},
		{53,71,44,65,25,43,91,52,97,51,14},
		{70,11,33,28,77,73,17,78,39,68,17,57},
		{91,71,52,38,17,14,91,43,58,50,27,29,48},
		{63,66,4,68,89,53,67,30,73,16,69,87,40,31},
		{4,62,98,27,23,9,70,98,73,93,38,53,60,4,23}
};

string iteratePath(string path,int pos) {
	if (path[pos] == 'L') {
		path[pos] = 'R';
		return path;
	}
	if (path[pos] == 'R' && pos!=0) {
		path[pos] = 'L';
		path = iteratePath(path, pos - 1);
	}
	return path;
}

long long unsigned int pathSum(string path) {
	int colPos = 0;
	long long unsigned int currSum = 0;
	for (int rowPos = 0; rowPos < path.size(); rowPos++) {
		if (colPos > rowPos) return 0;
		currSum += triang[rowPos][colPos];
		if (path[rowPos] == 'R') {
			colPos++;
		}
	}
	currSum += triang[14][colPos];
	return currSum;
}

int main() {

	/*	75
		95 64
		17 47 82
		18 35 87 10
		20 04 82 47 65
		19 01 23 75 03 34
		88 02 77 73 07 63 67
		99 65 04 28 06 16 70 92
		41 41 26 56 83 40 80 70 33
		41 48 72 33 47 32 37 16 94 29
		53 71 44 65 25 43 91 52 97 51 14
		70 11 33 28 77 73 17 78 39 68 17 57
		91 71 52 38 17 14 91 43 58 50 27 29 48
		63 66 04 68 89 53 67 30 73 16 69 87 40 31
		04 62 98 27 23 09 70 98 73 93 38 53 60 04 23
	

	string curr = "04 62 98 27 23 09 70 98 73 93 38 53 60 04 23";

	cout << "{";
	for (int i = 0; i < curr.size(); i++) {
		cout << curr[i] << curr[++i];
		i++;
		cout << ",";
	}
	cout << "},";

	*/

	long long unsigned int max = 0;
	long long unsigned int currSum = 0;
	string path = "LLLLLLLLLLLLLL";
	while (path != "RRRRRRRRRRRRRR") {
		currSum = pathSum(path);
		if (max < currSum) max = currSum;
		cout << path << " " << currSum << " " << max << endl;
		path = iteratePath(path, 13);
	}
	currSum = pathSum(path);
	if (max < currSum) max = currSum;
	
	cout << max << endl;

	return 0;
}