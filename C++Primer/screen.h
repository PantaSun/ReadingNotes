
class Screen{
  public:
      typedef string::size_type pos;
      Screen() = default;
      Screen(pos ht, pos wd, char c): height(ht), width(wd), contents(ht*wd, c) { }
      char get(){ return contents[cursor];} // 读取光标位置，隐式内联
      inline char get(pos ht, pos wd);      // 类内显示内联
      Screen &move(pos r, pos c);          // 类外显示内联，会在类外定义函数时会用`inline`关键字修饰
      Screen &set(char);
      Screen &set(pos, pos, char);
      Screen &display(std::ostream &os){ do_display(os); return *this;}
      const Screen &display(std::ostream &os) const { do_display(os); return *this;}
  private:
      void do_display(std::ostream &os) const { os << contents;}
      pos height = 0;
      pos width = 0;
      pos cursor = 0;
      std::string contents;
}
    inline // 类外显示内联
    Screen &Screen::move(pos r, pos c){
        pos row = r * width; // 计算行位置
        cursor = row + c;    // 在行内将光标移动到指定的列
        return *this;        // 以左值的方式返回对象
    }
    char Screen::get(pos r, pos c) const{  // 在类内部声明成inline
        pos row = r * width; // 计算行位置
        return contents[row+c]; // 返回给定列的字符
    }
    inline Screen &Screen::set(char c){
        contents[cursor] = c;
        return *this;
    }
    inline Screen &Screen::set(pos r, pos col, char c){
        contents[r*width+col] = c ;
        return *this;
    }
