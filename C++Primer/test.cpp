#include<iostream>
using std::cin;
using std::cout;
using std::endl;
int main(){
	class test
	{
		public:
			int func1(){return 11;}
			int func1() const {return 12;}	
	};

	test A;
	const test B;

	cout << "A.func1():" << A.func1() << endl;
	cout << "B.func1():" << B.func1() << endl;
	
	return 0;
}