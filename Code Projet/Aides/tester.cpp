//TESTS SUR LES POINTEURS
#include<iostream>

void tester(int &a) {
	a += 2;
}


int main() {

	int a = 5;

	tester(a);

	std::cout << "A was 5, now is " << a << std::endl;

	return 1;



}