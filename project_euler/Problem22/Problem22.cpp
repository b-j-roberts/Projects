#include <iostream>
#include <fstream>
#include <string>


class Name {
private:
	std::string name;
	int arr[];
public:
	Name(std::string name) {
		this->name = name;
		
	}
};

int main() {

	std::ifstream infile("names.txt");
	if (!infile.is_open()) {
		std::cout << "Error file did not open" << std::endl;
		exit(0);
	}


	return 0;
}