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



## Data Member 的布局

非静态数据成员在其类对象中的排列顺序和其在类内被声明的顺序一样。在非静态数据成员的声明之间加入几个静态成员的声明不会影响上述顺序，因为静态数据成员不会被放进对象布局之内。

C++ Standard要求在同一个access section（也就是public、private、protected等区段）中，members的排列顺序只需符合“较晚出现的members在类对象中有较高的地址”这一条即可。这个的意思就是说各个非静态数据成员不一定是紧挨在一起（连续）排列。什么东西可能出现在两个非静态数据成员之间呢？那就是用于alignment机制的填补bytes。

从之前所学可知，编译器还会合成一些数据成员加入到对象中，比如说vptr。而对于vptr的顺序，有可能是在所有显示声明的成员之后，也有可能放在一个类对象的最前端。

C++ Standard允许编译器把那些内部生成的成员自由地放在任何位置上，甚至放在显示声明出来的成员之间。

C++ Standard也允许不同的access section之中的数据成员自由排列，不用在乎它们在类中声明的顺序。这里包括同一类型的access section但不同的access section之间。

目前各家编译器都是把一个以上的access section 连锁在一起，按照声明的顺序成为一个连续区块。access section的多少不会造成额外的负担，一个section中的8个成员，或是这8个成员在8个section中所得到的类对象是一样大的。

## Data Member 的存取

#### 一个问题

```c++
// 假设有一个类
class Point3d
{
 public:
    float x;
    static list<Point3d *> *freeList;
    float y;
    static const int chunkSize =250;
    float z;
}
```

```
Point3d origin;
Point3d * pt = &origin;
origin.x = 0.o;
pt->x = 0.0
```

如上述代码所示，通过一个指向对象的指针和通过该对象来存取member，有什么差异？

### Static Data Members

每一个Static data member只有一个实例，放在程序的data segment之中。

每次程序存取Static member时，就会被内部转化为对该唯一extern实例的直接参考操作。举例：

```c++
// origin.chunkSize = 250;
Point3d::chunkSize = 250;
// pt->chunkSize = 250;
Point3d::chunkSize = 250
```

也就是说这是C++语言中“通过一个指向对象的指针和通过该对象来存取member，结论完全相同”的唯一一种情况。使用点运算符（member selection operator）对Static data member进行存取操作只是一种文法上的便宜行事而已。因为static data member并不在类的对象中，因此存取不需要通过类的对象。

若chunkSize是从复杂的继承关系中继承而来的也不会影响这种操作，因为在程序之中对于static member来说还是只有唯一一个实例。

若static data member被一个函数调用（也就是说这个函数可以返回一个Point3d类型的对象或对象的引用），例如`foo().chunkSize = 250;`，foo()函数是否会运行？在ARM中没有规定foo是否必须被求值；而在C++ Standard中要求foo必须求值，虽然其结果并无用处，因为最终还是会转化为`Point3d::chunkSize = 250;`

对static data member取地址，会得到一个指向其数据类型的指针，而不是一个指向其class member的指针。因为static member不在一个class object之中。例如，`&Point3d::chunkSzie;`会获得`const int *`类型的内存地址。

当两个类都声明了一个static member freeList，那么当他们都放在程序的data segment时，就会导致名称冲突，这是编译器就会暗中对每个static data member进行编码（name-mangling），这样就可以获得独一无二的程序识别码。

不管如何进行name-mangling，都要遵守两个规则：

1. 一个算法，推导出独一无二的名称
2. 允许根据推导出的名称可以恢复原来的名称，以便于编译系统或环境工具要跟使用者交流



### Nonstatic Data Member

要对一个Nonstatic data member进行存取操作，编译器需要把class object的地址加上data member 的偏移位置（offset），例如`&origin.y`相当于`&origin + (&Point3d::y - 1)`，这里的“-1”是为了区别**“一个指向data member的指针，用以指向class的第一个member”和“一个指向data member的指针，没有指向class的第一个member”**，具体后面会说，**目前我是没看懂这句话**。

- 通过对象、引用或指针存取一个nonstatic data member的成本，其效率和存取一个C struct member或一个nonderived class的member（单一继承或多重继承）是一样的（多重继承的时候，members的位置在编译时就固定了，存取members只是一个简单的offset运算）。
- 通过引用或指针比通过对象存取一个virtual base class的member的速度会稍慢一点（我们不知道这个引用或指针指向哪一种class type，也就不知道编译期这个member真正的offset的位置，所以这个存取操作必须延迟至执行期，经由一个额外的间接导引，才能够解决）。但是如果不使用指针而直接使用对象就不会有这些问题，因为该对象类型肯定是派生类类型的。而该派生类继承自基类的data member的在派生类内的偏移量在编译时期已经确定了。