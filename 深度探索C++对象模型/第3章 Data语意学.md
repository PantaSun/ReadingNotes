## 前言

已知有如下代码：

```C++
class X{ };
class Y: public virtual X{ };
class Z: public virtual Y{ };
class A: Y, Z{ };
```

对每个class进行sizeof操作，在一些旧的编译器（32位机器）上的结果是:

- sizeof(X) ：1
- sizeof(Y) ：8
- sizeof(Z) ：8
- sizeof(A) ：12

而在一些新的编译器（如Visual C++5.0，32位机器）中结果却是：

- sizeof(X) ：1
- sizeof(Y) ：4
- sizeof(Z) ：4
- sizeof(A) ：8

由此可见出现这两种结果和编译器有很大关系：

- 对于旧编译器来说：
  - sizeof(X)：按理说class中并没有数据成员，所以大小应为0才对；事实上它并不是空的，它有一个隐藏的1byte，这个byte是被编译器安插进去的一个char。这使得class X的两个objects得以在内存中配置独一无二的地址。
  - sizeof(Y)：Y的大小为8，这个大小和机器有关，也和编译器有关。有三个因素导致这个class的大小为8：
    - 语言本身的负担：因为使用虚继承，所以派生类中要有一个指针，要么指向virtual base class subobject要么指向一个相关表格。而指针大小在32位机器上是4字节。
    - 编译器对特殊情况提供的优化处理：也就是class X的1byte大小也出现在class Y上。一般放在派生类固定部分的尾端。
    - alignment（对齐）：在大部分机器上，聚合的结构体会受到alignment的限制。为了更有效的在内存中存取，会将结构体所占内存调整为某数的倍数，这里是4bytes（32位机器）。目前来看Y的大小为5，根据alignment机制，要补3bytes使得class Y的大小为8bytes。
  - sizeof(Z)：同理class Y
  - sizeof(A)：由于一个virtual base class subobject只会在继承串中出现一份实例，所以A的大小是1+4+4=9，再根据alignment机制，最终大小为12。
- 对于新编译器：
  - sizeof(X)：没有变化
  - sizeof(Y)：针对上述三个因素中的第二点，新编译器采用**empty virtual base class策略**。在这个策略下，一个empty virtual base class被视为派生类最开头的部分（用虚基类的1byte代替派生类自己的那1byte），也就是说没有花费任何额外空间。这就节省了1byte。因此Y的大小为4bytes。
  - sizeof(Z)：同上述Y，大小为4bytes。
  - sizeof(A)：这时候A的大小就是Y的大小加Z的大小为8bytes 。



## Data Member 的绑定

**一段易错代码 **

```c++
// 某个foo.h头文件，从某处含入
extern float x;
class Point3d
{
 public:
    Point3d(float, float, flaot);
    float X()const{return x;}
 private:
    float x, y, z;
};

```

问题是函数Point3d::X()这个函数返回的x是global的还是class内的？从目前来看回答是“返回class内的x”肯定是正确的，但是在最早的C++编译器上，返回的x是global的。为了消除这种错误：

**早期C++两种防御性程序设计风格：** 

1. 把所有的data member放在class声明起头处，以确保正确的绑定：

```c++
class Point3d{
    //将class声明起头处放data member
    float x,y,z;
public:
    folat X() const{return x;}
    //etc
};
```

2.把所有的inline function，不管大小都放在class的声明之外：

```c++
lass Point3d{
public:
    //class的声明处放inline function
    Point3d();
    float X() const;
};
inline float
Point3d::X()const
{
  return x;  
}
```

这个古老的规则被称为”member rewriting rule”，大意是：一个inline函数实体，在整个class声明未被完全看见之前，是不会被评估求值的。但是如果一个inline函数在声明之后被立即定义，那么还是会被评估求值。 
但是对于member function的argument list并不为真。考虑nested(嵌套的) typedef:

```c++
include <iostream>

using namespace std;
typedef float length;
class Point3d {
public:
    //length被决议为global
    //val被决议为Point3d::val
    void mumble(length val) { val = val; }
    length mumble() {
        cout << typeid(val).name() << endl;
        return _val;
    }
private:
    typedef int length;
    length _val;
};
int main()
{
    Point3d p;
    p.mumble(); //i(int)
    cout << typeid(p.mumble()).name();  //f(float)
    return 0;
}

```

所以，总是把“nested type声明”放在class的起始处，确保绑定的正确性。