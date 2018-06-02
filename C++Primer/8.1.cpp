#include <iostream>
#include <stdexcept>
using namespace std;
// using std::cin;
// using std::cout;
// using std::endl;

istream& test(istream& in){
	int v;
	while(in >> v, !in.eof()){
		if(in.bad())
			throw runtime_error("I/O流错误！");
		if(in.fail()){
			cerr << "数据错误" << endl;
			in.clear();
			in.ignore(100, '\n');
			continue;
		}
		cout << v << endl;
	}
	in.clear();
	return in;
}

int main(){
	cout << "请输入一些整数，按Ctrl+Z结束" << endl;
	test(cin);
	return 0;
}