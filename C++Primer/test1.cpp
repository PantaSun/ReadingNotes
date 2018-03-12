#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ostream;
 // Screen 类成员函数举例
class Screen  
{  
    // friend void Window_mgr::clear(Screen_index i);  
public:  
    using pos = string::size_type;  
    mutable unsigned counter = 0;  
  
    //constructors  
    Screen() = default;  
    Screen(pos ht, pos wd, char c) :height(ht), width(wd), contents(ht*wd, c) {}  
  
    //operations  
    char get() const { return contents[cursor]; }  
    inline char get(pos ht, pos wd) const;  
    Screen &move(pos r, pos c);  
    Screen &set(char);  
    Screen &set(pos, pos, char);  
  
    //output methods  
    Screen &display(ostream &os) { do_display(os); return *this; }  
    const Screen &display(ostream &os) const { do_display(os); return *this; }  

private:  
    pos cursor = 0;  
    pos height = 0;
    pos width = 0;  
    string contents;  
    void do_display(ostream &os) const { os << contents; }  
};  
  
char Screen::get(pos r, pos c) const{  // 在类内部声明成inline
        pos row = r * width; // 计算行位置
        return contents[row+c]; // 返回给定列的字符
}
    
  
inline Screen &Screen::move(pos r, pos c){ // 类外显示内联
        pos row = r * width; // 计算行位置
        cursor = row + c;    // 在行内将光标移动到指定的列
        return *this;        // 以左值的方式返回对象
}
  
inline Screen &Screen::set(char c){  
    contents[cursor] = c;  
    return *this;  
}  
  
inline Screen &Screen::set(pos r, pos c, char ch){  
    contents[r*width + c] = ch;  
    return *this;  
}  


int main()
{
  /* code */
  Screen myScreen(5, 5, 'X');
  myScreen.move(4, 0).set('#');
  cout << endl;
  myScreen.display(cout);
  cout << endl; 
  return 0;
}