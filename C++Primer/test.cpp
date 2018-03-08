#include<iostream>
using std::cin;
using std::cout;
using std::endl;
int main(){
	class test
	{
	public:
		int func1(){return 11;}
		int func2() const {return 12;};	
	};
	test A;
	cout << A.func1() << endl;
	cout << A.func2() << endl;

	const test B;
	//cout << B.func1() << endl;
	cout << B.func2() << endl;

	return 0;
}