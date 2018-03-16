#include<iostream>
#include <typeinfo>
using std::cin;
using std::cout;
using std::endl;

typedef std::string Type;
Type initVal();

class Exercise{
	public:
		typedef double Type;
		Type setVal(Type parm);
		Type initVal(){
			::Type s1 = "initVal()"; 
			cout << s1 << endl;
			return val; }
			
			
	private:
		Type val = 0.0;
};

Exercise::Type Exercise::setVal(Type parm){
	val = parm + initVal();
	::Type s2 = "setVal()";
	cout << s2 << endl;
	return val;
}

int main(){
	Exercise A;
	cout << A.setVal(10) << endl;
	
	return 0;
}