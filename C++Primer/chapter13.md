## 第十三章：拷贝控制

- 在定义一个类时，可以显式或隐式的指定此类的对象拷贝、移动、赋值和销毁时做什么。
- 通过五种特殊的成员函数来控制这些操作：
  - 拷贝构造函数
  - 拷贝赋值运算符
  - 移动构造函数
  - 移动赋值运算符
  - 析构函数

### 拷贝赋值与销毁

#### 拷贝构造函数

- 如果一个构造函数的第一个参数是自身类类型引用，且任何额外参数都有默认值，则此构造函数是拷贝构造函数。

  ```c++
  class Foo{
  public:
  	Foo(); //默认构造函数
  	Foo(const Foo&); //默认拷贝构造函数
      //....
  };
  ```

- 如果我们没有为一个类定义拷贝构造函数，编译器会为我们定义一个。

- 与合成默认构造函数不同，即使我们定义了一个其他构造函数，编译器也会为我们合成一个拷贝构造函数。

- 一般情况下合成拷贝构造函数会将其参数的成员逐个拷贝到正在创建的对象中：

  - 编译器从给定对象依次将每个非static成员拷贝到正在创建的对象中
  - 对类类型成员，会使用其拷贝构造函数来拷贝
  - 内置类型的成员直接拷贝
  - 数组成员是逐元素地拷贝
  - 如果数组的元素是类类型，则使用元素的拷贝构造函数拷贝

- 拷贝初始化与直接初始化

  ```c++
  string dots(10, ','); // 直接初始化
  string s(dots);		  // 直接初始化
  string s2 = dots;	  // 拷贝初始化
  string null_book = "9-99-99999-9";  // 拷贝初始化
  string nines = string(100, '9');    // 拷贝初始化
  ```

- 拷贝初始化不仅在用等号=定义变量时会发生，还有其他情况：

  - 将一个对象作为实参传递给一个非引用类型的形参时，这里解释了为什么拷贝构造函数自己的参数必须是引用类型。如果不是引用类型，则调用永远不会成功：为了调用拷贝构造函数，我们必须拷贝它的实参，但为了拷贝实参，就又需要调用拷贝构造函数，一直循环下去。
  - 从一个返回类型为非引用类型的函数返回一个对象时
  - 用花括号列表初始化一个数组中的元素或一个聚合类中的成员时

- 应用

  ```c++
  class HasPtr{
  public:
  	HasPtr(const std::string &s = std::string()):ps(new std::string(s)), i(0){ }
      HasPtr(const HasPtr &);
  private:
  	std::string *ps;
  	int i;
  };
  HasPtr :: HasPtr(const HasPtr &hp){
      ps = new string(*hp.ps);//拷贝ps指向的对象，而不是拷贝指针本身
      i = hp.i;
  }
  ```

  

#### 拷贝赋值运算符

- 与类控制其对象如何初始化一样，类也可以控制其对象如何赋值。

- 若类未定义自己的拷贝赋值运算符，编译器会为它合成一个。

- 赋值运算符通常应该返回一个指向其左侧运算对象的引用。

- 拷贝赋值运算符本身是一个重载的赋值运算符，定义为类的成员函数，左侧运算对象绑定到隐含的this参数。

  ```c++
  class Foo{
      public:
      Foo& operator=(cosnt Foo&); // 赋值运算符
      //...
  }
  ```

- 应用

  ```c++
  class HasPtr{
  public:
  	HasPtr(const std::string &s = std::string()):ps(new std::string(s)), i(0){ }
      HasPtr(const HasPtr &);            // 拷贝构造函数
      HasPtr& operator= (const HasPtr&); // 赋值运算符
  private:
  	std::string *ps;
  	int i;
  };
  HasPtr :: HasPtr(const HasPtr &hp){
      ps = new string(*hp.ps);// 拷贝ps指向的对象，而不是拷贝指针本身
      i = hp.i;
  }
  HasPtr& HasPtr :: operator= (const HasPtr& hp){
      auto newps = new string(*hp.ps); // 拷贝指针指向的对象
      delete ps;  // 销毁原string
      ps = newps; // 指向新string
      i = hp.i;   // 使用内置的int赋值
      return *this; //返回一个此对象的引用
  }
  ```

#### 析构函数

- 释放对象使用的资源，并销毁对象的非static数据成员。

- 析构函数是一个成员函数，名字由波浪线接类名构成。没有返回值，也不接受参数。

  ```
  class Foo{
  public:
  	~Foo(); //析构函数
  };
  ```

- 应用

  ```c++
  class HasPtr{
  public:
  	HasPtr(const std::string &s = std::string()):ps(new std::string(s)), i(0){ }
      HasPtr(const HasPtr &);            // 拷贝构造函数
      HasPtr& operator= (const HasPtr&); // 赋值运算符
      ~HasPtr(){ delete ps;} 
  private:
  	std::string *ps;
  	int i;
  };
  HasPtr :: HasPtr(const HasPtr &hp){
      ps = new string(*hp.ps);// 拷贝ps指向的对象，而不是拷贝指针本身
      i = hp.i;
  }
  HasPtr& HasPtr :: operator= (const HasPtr& hp){
      auto newps = new string(*hp.ps); // 拷贝指针指向的对象
      delete ps;  // 销毁原string
      ps = newps; // 指向新string
      i = hp.i;   // 使用内置的int赋值
      return *this; //返回一个此对象的引用
  }
  ```

#### 综合练习

```c++
//
//  main.cpp
//  CPP04
//  练习拷贝赋值与销毁
//

#include <iostream>
#include <vector>
using namespace std;
struct X{
    X(){ cout << "构造函数 X()" << endl;}
    X(const X&){ cout << "拷贝构造函数 X(const X&)" << endl;}
    
    X& operator=(const X &rhs){
        cout << "拷贝赋值运算符 X & operator=(const X &)"<< endl;
        return *this;}
    ~X(){cout << "析构函数~X()" << endl;}
};
void f1(X x){}
void f2(X &x){}
X f3(){
    X x;
    return x;
}
X& f4(){
    X x4;
    return x4;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    cout << "局部变量：" << endl;
    X x1;
    cout << endl;
    cout << "作为非引用参数传递：" << endl;
    f1(x1);
    cout << endl;
    cout << "作为引用参数传递："<< endl;
    f2(x1);
    cout << endl;
    cout << "作为非引用返回值传递："<< endl;
    f3();
    cout << endl;
    cout << "作为引用返回值传递："<< endl;
    f4();
    cout << endl;
    cout << "动态分配：" << endl;
    X *p = new X;
    cout << endl;
    cout << "添加到容器中：" << endl;
    vector<X> test;
    test.push_back(x1);
    cout << endl;
    cout << "释放动态分配对象：" << endl;
    delete p;
    cout << endl;
    cout << "间接初始化对象：" << endl;
    X x2 = x1;
    cout << endl;
    cout << "赋值操作：" << endl;
    x2 = x1;
    cout << endl;
    cout << "程序结束！" << endl;
    return 0;
}

```

输出结果：

```
Hello, World!
局部变量：
构造函数 X()

作为非引用参数传递：
拷贝构造函数 X(const X&)
析构函数~X()

作为引用参数传递：

作为非引用返回值传递：
构造函数 X()
析构函数~X()

作为引用返回值传递：
构造函数 X()
析构函数~X()

动态分配：
构造函数 X()

添加到容器中：
拷贝构造函数 X(const X&)

释放动态分配对象：
析构函数~X()

间接初始化对象：
拷贝构造函数 X(const X&)

赋值操作：
拷贝赋值运算符 X & operator=(const X &)

程序结束！
析构函数~X()
析构函数~X()
析构函数~X()
Program ended with exit code: 0
```





