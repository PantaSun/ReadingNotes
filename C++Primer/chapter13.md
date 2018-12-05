## 第十三章：拷贝控制

- 在定义一个类时，可以显式或隐式的指定此类的对象拷贝、移动、赋值和销毁时做什么。
- 通过五种特殊的成员函数来控制这些操作：拷贝控制操作
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

- 拷贝构造函数必须是一个引用类型：如果不是引用类型，则调用永远不会成功：为了调用拷贝构造函数，我们必须拷贝它的实参，但为了拷贝实参，就又需要调用拷贝构造函数，这样一直循环下去，就是死循环。这里解释了为什么拷贝构造函数自己的参数必须是引用类型。

- 拷贝构造函数在几种情况下都会被隐式的使用。因此拷贝构造函数通常不应该是explicit的，即不应该阻止隐式转换。

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

- 当使用拷贝初始化时，我们要求编译器将右侧运算对象拷贝到正在创建的对象中，如果需要还要进行类型转换。

- 拷贝初始化不仅在用等号=定义变量时会发生，还有其他情况：

  - 将一个对象作为实参传递给一个非引用类型的形参时，这里解释了为什么拷贝构造函数自己的参数必须是引用类型。如果不是引用类型，则调用永远不会成功：为了调用拷贝构造函数，我们必须拷贝它的实参，但为了拷贝实参，就又需要调用拷贝构造函数，一直循环下去。
  - 从一个返回类型为非引用类型的函数返回一个对象时
  - 用花括号列表初始化一个数组中的元素或一个聚合类中的成员时。

- 拷贝初始化的限制：当传递一个实参或从函数返回一个值时，我们不能隐式使用一个explicit构造函数。若要使用则需要显示的使用：

  ```c++
  vector<int> v1(10); // 正确：直接初始化
  vector<int> v2 = 10; // 错误：接受大小参数的构造函数是explicit的
  void f(vector<int>){}; // f的参数进行拷贝初始化
  f(10); //错误：不能使用一个explicit的构造函数拷贝一个实参
  f(vector<int>(10)); // 正确：显示的使用，即从一个int直接构造一个临时vector
  ```

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

- 标准库通常要求保存在容器中的类型要具有赋值运算符，且其返回值是左侧运算对象的引用。

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

- 由于析构函数不接受参数，所以对于一个给定的类，只会有唯一一个析构函数。

  ```c++
  class Foo{
  public:
  	~Foo(); //析构函数
  };
  ```

##### 析构函数完成什么工作

- 在析构函数中，首先执行函数题，然后销毁成员。成员按照初始化顺序的逆序销毁。

- 通常析构函数释放对象在生存期分配的所有资源。
- 析构部分是隐式的。成员销毁时发生什么完全依赖于成员的类型。
- 销毁类类型的成员需要执行成员自己的析构函数。
- 隐式销毁一个内置指针类型的成员不会delete它所指向的对象。

##### 何时会调用析构函数

- 无论何时一个对象被销毁，就会自动调用其析构函数
  - 变量离开其作用域
  - 当一个对象被销毁，其成员被销毁
  - 容器被销毁时，其元素被销毁
  - 对于动态分配的对象，当指向它的指针应用delete运算符时被销毁
  - 对于临时对象，当创建完它的完整表达式结束时被销毁。
- 析构函数体自身并不直接销毁成员，成员是在析构函数体之后隐含的析构阶段中被销毁的。
- 在整个对象销毁过程中，析构函数体作为成员销毁步骤之外的另一部分而进行的。

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



#### 三/五法则

##### 需要析构函数的类也需要拷贝和赋值操作

- 若为某个类定义一个析构函数，但使用合成版本的拷贝函数和赋值函数：

  ```c++
  class H{
  public: 
      H(const std::string &s = std::string()):ps(new std::string(s)), i(0){}
      ~H(){delete ps;}
  private:
  	std::string *ps;
  	int i;
  };
  ```

  上述代码中使用的是合成的拷贝和赋值函数，这些函数简单拷贝指针成员，这意味着多个H对象可能指向相同的内存：

  ```c++
  H f(H hp){  // H 是传值参数，将被拷贝
      H ret = hp;  // 拷贝给定的H
      return ret; // ret和hp被销毁
  }
  ```

  当f返回时，hp和ret都被销毁，这两个对象都会执行析构函数，即delete ret和hp中指针成员。但这两个对象包含相同的指针值。这样就会对该指针执行两次delete，这显然是错误的。另外当将对象传递给f后，该对象也因为f函数的原因指向无效内存。

##### 需要拷贝操作的类也需要赋值操作，反之亦然

然而不一定也需要析构函数。

#### 使用=default

- 可以通过将拷贝控制成员定义为=default来显示地要求编译器生成合成的版本。

  ```c++
  class Sales_data{
  public:
  	Sales_data() = default;
  	Sales_data(const Sales_data&) = default;
  	Sales_data& operator=(const Sales_data &);
  	~Sales_data() = default;
  };
  Sales_data & Sales_data::operator=(const Sales_data&) = default;
  ```

#### 阻止拷贝

- 在某些情况下，拷贝构造函数和赋值构造函数没有用，甚至可能起反作用，这时候就需要采用某种方式阻止拷贝或赋值。当然不是说不定义这些操作，因为我们不定义，编译器也会生成合成版本，即这中策略无效。

##### 定义删除函数

- 在函数的参数列表后面加上=delete来指出我们希望将它定义为删除的函数来阻止拷贝。

  ```c++
  struct NoCopy{
      NoCopy() = default;
      NoCopy(const NoCopy&) = delete; // 阻止拷贝
      NoCopy &operator=(const NoCopy&) = delete;  //阻止赋值
  	～NoCopy() = default;
  };
  ```

  =delete通知编译器或读我们代码的读者，我们不希望定义这些成员。

- =delete必须出现在函数第一次声明的时候，这与=default是不同的。

- 还有一个不同之处就是，可以对任函数指定=delete（析构函数除外），而=default只能对编译器可以合成的默认构造函数或拷贝控制成员还用。

##### 析构函数不能是删除函数

- 如果析构函数被删除，就无法销毁此类型的对象了。

- 对于析构函数已经删除的类型，不能定义该类型的变量或释放指向该类型动态分配对象的指针。

- 也就是说删除了析构函数的类型，虽然不能定义这种类型的变量或成员，但可以动态分配这种类型对象。但是不能释放这些对象：

  ```c++
  struct N{
      N() = default;
      ~N() = delete;
  };
  
  N n; // 错误：N的析构函数是删除的
  N *p = new N(); // 正确：但是不能delete p
  delete p; // 错误：N的析构函数已经删除
  ```

##### 合成的拷贝控制成员可能是删除的

- 对某些类来说，编译器将如下情况的合成的成员定义为删除的函数：
  - 如果类的某个成员的**析构函数**是删除的或不可访问的（例如是private的），则类的**合成析构函数**被定义为删除的。
  - 如果类的某个成员的**拷贝构造构函数**是删除的或不可访问的，则类的**合成拷贝构造函数**被定义为删除的。如果类的某个成员的**析构函数**是删除的或不可访问的，则类的**合成拷贝构造函数**也被定义为删除的。
  - 如果类的某个成员的**拷贝赋值运算符**是删除的或不可访问的，或类是有一个const的或引用成员，则类的**合成拷贝赋值运算符**被定义为删除的。
  - 如果类的某个成员的**析构函数**是删除的或不可访问的，或是类有一个引用成员，它没有类内初始化器，或是类有一个cosnt成员，它没有类内初始化器且其类型未显示定义默认构造函数，则该类的**默认构造函数**被定义为删除的。
- 总的来说，如果一个类有数据成员不能默认构造、拷贝、赋值或销毁，则对应的成员函数将被定义为删除的。

##### private 拷贝控制

- 举例

  ```c++
  class P{
      //无访问说明符：接下来的成员默认是private的
      P(const P&);
      P &operator=(const P&);
  public:
  	P() = default;
  	~P();
  }
  ```

  上述代码中，析构函数是public的所以，用户可以定义P类型的对象。但是拷贝构造函数和拷贝赋值运算符是private的，用户代码将不能拷贝这个类型的对象。但是友元和成员函数仍旧可以拷贝对象。

  如果也不想让友元和其他成员函数进行拷贝，可以只在private声明，但不定义它们，这样友元和其他成员在进行拷贝（试图访问一个未定义的成员）时就会导致一个连接时错误。这样就可以完全阻止拷贝了。

### 拷贝控制和资源管理

- 可以定义拷贝操作使类的行为看起来像一个值或一个指针：
  - 类的行为像一个值，意味着它应该也有自己的状态。当拷贝一个像值的对象时，副本和原来对象是完全独立的。改变副本不会改变原对象，反之亦然。
  - 行为像指针的类则共享状态。当拷贝一个这种类的对象时，副本和原对象使用相同的底层数据。改变副本也会改变对象，反之亦然。

##### 行为像值的类

- 例子

  ```c++
  class H{
  public:
  	H(const string &s = string()):ps(new string(s), i(0)){}
  	H(const H &p): ps(new string(*p.ps), i(p.i)){}
  	H &operator=(const H&);
  	~H(){delete ps;}
  private:
  	string *ps;
  	int i;
  };
  H & H::operator=(const H &rhs){
      auto newp = new string(*rhs.ps);
      delete ps;
      ps = newp;
      i = rhs.i;
      return *this;
  }
  
  ```

  这里先拷贝底层string，再释放旧内存的顺序很重要，当将一个对象赋予它自身时，如果先释放旧内存，那么由于*this和rhs是同一个对象，则delete ps也意味着将rhs.ps释放，后续就无法拷贝底层string了。

##### 定义行为像指针的类

- 对于行为像指针的类，需要定义拷贝构造函数和拷贝赋值运算符，来拷贝指针成员本身而不是它指向的string。

- 本例子中析构函数不能单方面释放关联的string。

- 只有当最后一个指向string的H销毁时，它才可以释放string。

- 例子

  ```c++
  class H{
  public:
  	H(const string &s = string()): ps(new string(s), i(0), use(new size_t(1))){}
  	H(const H &p):ps(p.ps), i(p.i), use(p.use){ ++*use;}
  	H & operator=(const H&);
  	~H();
  private:
  	string *ps;
  	int i;
  	size_t *use;
  };
  H& H::operator=(const H &rhs){
      ++*rhs.use;
      if(--*use == 0){
          delete ps;
          delete use;
      }
      ps = rhs.ps;
      i = rhs.i;
      use = rhs.use;
      return *this;
  }
  H::~H(){
      if(--*use == 0){
          delete ps;
          delete use;
      }
  }
  ```

- 这里加入了引用计数use，工作方式：

  - 除了初始化对象外，每个构造函数还要创建一个引用计数，来记录有多少个对象与正在创建的对象共享状态。当创建一个对象时，只有一个对象共享状态，因此计数器初始化为1。
  - 拷贝构造函数不分配新的计数器，而是拷贝给定对象的数据，包括计数器，然后递增计数器。
  - 析构函数递减计数器，若递减后的计数器为0，则析构函数释放状态。
  - 拷贝赋值运算符递增右侧运算对象的计数器，递减左侧运算对象的计数器。如果左侧运算对象的计数器在递减后变为0，则就必须销毁原状态。

- 引用计数保存着动态内存中。当创建一个对象时，就分配一个新的计数器；但是当拷贝或赋值对象时，就拷贝指向计数器的指针。这样就能保证副本和原对象指向相同的计数器。

### 交换操作

- 如果一个类定义了自己的swap函数，那么算法将使用自定义版本。否则，算法将使用标准库定义的swap。

- 例子：

  ```c++
  class H{
      friend void swap(H&, H&);
      // 	其他同上
  };
  
  inline void swap(H &lhs, H &rhs){
      using std::swap;
      swap(lhs.ps, rhs.ps);
      swap(lhs.i, rhs.i);
  }
  
  ```

- 注意这里没有使用std::swap这这种方式，因为这种方式会限制函数只调用标准库的swap，但有时候要交换的对象是类类型的，可能要使用该类自己的swap。所以只需要在上边声明一下`using std::swap;`就可以了，至于到底使用哪个版本的swap编译器自己会判断。

##### 在赋值运算符中使用swap

- 拷贝并交换：

  ```c++
  H & H::operator=(H rhs){
      swap(*this, rhs);
      return *this;
  }
  ```

  这里使用传值的方式，也就是说rhs是右侧运算对象的一个副本。在这个版本的赋值运算符中，左侧运算对象原来的保存的指针会存入rhs中，并将rhs中原来的指针存入*this中。在运算结束之后由于rhs是一个局部变量会被销毁。但它只是右侧运算对象的一个副本，因此销毁了也不会影响到右侧运算对象。

  而且这中方式可以很好的处理自赋值情况。



### 动态内存管理类

- 以一个简化版vector类为例：简化版不使用模版，只用于string，称为StrVec。
- 主要思想：每次添加新元素时要检是否有空间容纳更多元素。如果有，就会在下一个可用位置构造一个对象。如果没有，vector就会重新分配空间：它获得新的空间，将已有元素移动到新的空间中，释放旧空间，并添加新元素。

### 对象移动

- 在重新分配内存的过程中，从旧内存将元素拷贝到新内存是不必要的，更好的方式是移动元素。
- 使用移动而不使用拷贝的另一个原因是IO类或uniqye_ptr这样的类都包含不能被共享的资源，因此此类对象不能拷贝但可以移动。

#### 右值引用

- 必须绑定到右值的引用，用&&来获得右值引用
- 右值引用只能绑定到一个将要销毁的对象。
- 一般而言，一个左值表达式表示的是一个对象的身份，而一个右值表达式表示的是对象的值。
- 左值不能绑定的要求转换的表达式、字面常量或返回右值的表达式。
- 右值可以绑定到这类表达式上，但不能将一个右值引用绑定的左值上。

##### 左值持久；右值短暂

- 左值有持久的状态，而右值要么是字面常量，要么是在表达式求值过程中创建的临时对象。

- 使用右值引用的代码可以自由地接管所引用的对象的资源。

##### 变量是左值

- 变量是左值，因此不能将一个右值引用直接绑定到一个变量上，即使这个变量是右值引用类型也不行。

##### 标准move函数

- 虽然不能将一个右值直接绑定到一个左值上，但可以显示地将一个左值转换为对应的右值引用类型，move函数定义在头文件utility中。

  ```c++
  int &&rr3 = std::move(rr1);
  ```

- 这里rr1就成了移后源对象，可以销毁它，也可以赋予它新值，但不能使用移后源对象的值。

#### 移动构造函数和移动赋值运算符

##### 移动构造函数

- 例子：

  ```c++
  StrVec::StrVec(StrVec &&s) noexcept 
  :elements(s.elements), first_free(s.first_free), cap(s.cap)
  {
   	s.elements = s.first_free = s.cap = nullptr;   
  }
  ```

  - 这里noexcept是用来通知标准库该构造函数不抛出任何异常。
  - 若函数的声明和定义是分开写的，即函数的定义在类外，则需要在定义和声明中都指定noexcept。
  - 移动构造函数不分配任何新内存；它接管给定的StrVec中的内存。
  - 接管之后将给定对象中的指针都置为nullptr。

  

##### 移动赋值运算符

- 例子：

  ```c++
  StrVec & StrVec::operator=(StrVec &&rhs) noexcept{
      if(this != %rhs){ //检查自赋值
          free(); //释放已有元素
          elements = rhs.elements; //从rhs接管资源
          first_free = rhs.fisrt_free;
   		cap = rhs.cap;
          // 将rhs置于可析构状态
          rhs.elements = rhs.first_free = rhs.cap = nullptr;
      }
      return *this;
  }
  ```

##### 移后源对象必须可析构

- 编写移动操作时，必须确保移后源对象进入一个可析构的状态。
- 还要保证移后源对象仍然是有效的，即可以安全的为其赋予新值或者可以安全的使用而不依赖其当前值。

##### 合成移动操作

- 若一个类定义了自己的拷贝构造函数、拷贝赋值运算符或析构函数，编译器就不会为它合成移动构造函数和移动赋值运算符了。
- 一般来说如果一个类没有移动操作，通过正常的函数匹配，类会使用对应的拷贝操作来代替移动操作。
- 只有当一个类没有定义任何自己版本的拷贝控制成员，且类的每个非static数据成员都可以移动时，编译器才会为它合成移动构造函数或移动赋值运算符。
- 编译器可以移动内置类型的成员。
- 移动操作永远不会隐式定义为删除的函数。

##### 移动右值，拷贝左值，但是如果没有移动构造函数，右值也被拷贝。

#### 右值引用和成员函数







