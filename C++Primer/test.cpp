#include<iostream>
using std::cin;
using std::cout;
using std::endl;
int main(){
	class test
	{
		public:
			typedef std::string::size_type pos;
			void dummy_fcn(pos height){
				cursor = width * height;
			}
			pos get_cursor(){
				return cursor;
			}
		private:
			pos cursor = 0;
			pos height = 0;
			pos width = 0;
	};

	test A;
	A.dummy_fcn(10);
	cout << A.get_cursor();
	
	return 0;
}