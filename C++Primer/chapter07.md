## 第七章 类

- 类的基本思想**数据抽象**和**封装**
  - 数据抽象：是一种依赖于**接口**和**实现**分离的编程技术。
    - 接口：包括用户（这里“用户”指的是类的使用者，与类的设计者区分开）所能执行的操作。
    - 实现：包括类的数据成员、负责接口实现的函数体以及定义类所需的各种私有函数。
  - 封装：实现了类的接口和实现的分离。
- 类想要实现数据抽象和封装，首先要定义一个抽象数据类型。

### 7.1 定义抽象数据类型

- 抽象数据类型：可以通过接口使用这个对象，但不能直接访问该对象的数据成员。一个普通的结构体不是一个抽象数据类型，因为它允许类的用户直接访问它的数据成员，并且要求有用户来编写操作。要想把一个普通的结构体变为抽象数据类型，就 需要定义一些操作以供类的用户使用，一旦定义了操作，就可以封装结构体的数据成员了。

#### 7.1.2 定义改进的Salas_data类

- 成员函数的定义必须在类的内部，它的定义则既可以在在类的内部，也可以在类的外部。

  ```c++
  struct Sales_data{
    // 关于Sales_data对象的操作
    std::string isbn() const { return bookNo; }	// 定义在类内
    Sales_data &combins(const Sales_data); 		// 定义在类外
    double avg_price() const;                 	// 定义在类外
    // 数据成员
    std::string bookNo;
    unsigned units_sold = 0;
    double revenue = 0.0;
  };
  ```

- 引入`this`：当调用成员函数时，实际上实在替某个对象调用它。例如`isbn()`这个成员函数，它是如何返回`bookNo`的呢？

  ```c++
  Sales_data total;
  total.isbn();
  ```

  - 这里当`isbn`返回`bookNo`时，实际上它隐式的返回`total.bookNo`。
  - 成员函数通过`this`这个额外的隐式参数来访问调用它的对象。当我们调用一个成员函数时，用请求该函数的对象地址初始化`this`。
  - `this`的类型在默认情况是指向类类型非常量版本的常量指针，例如上述类中`this`的类型是`Sales_data *const`。
  - 在成员函数的内部可以直接使用调用该函数的对象的成员，而无须通过成员访问运算符来做到这一点，因为`this`所指的正是这个对象。任何对类成员的直接访问都被看成`this`的隐式引用。
  - 当`isbn`使用`bookNo`时，它隐式的使用`this`指向的成员，就像书写了`this->bookNo`一样。
  - 也可以把`isbn`成员函数定义如下，虽然没什么必要

  ```c++
  std::string isbn() const {return this->bookNo;}
  ```

- 引入`const`（常量）成员函数：

  - 在默认情况下`this`的类型是指向类类型非常量版本的常量指针，因此不能将`this`绑定到一个常量对象上，也就使得我们不能在一个常量对象上调用普通的成员函数。


  - 在`isbn`函数参数列表后面的`const`关键字的作用是修改隐式`this`指针的类型。

  - 由上述代码可知`isbn`是一个常量成员，因此该函数中的隐式`this`是指向常量的指针，所以常量成员函数不能修改调用它的对象的内容。也就是`isbn`可以读取调用它的对象的数据成员，但不能写入新值。

  - `isbn`函数表达的意思如下：但这只是伪码，因为不能显示的定义自己的`this`指针。

    ```c++
    std::string Sales_data::isbn(const Sales_data *const this){return this->isbn;}  
    ```


  - 常量对象，以及常量对象的引用或指针都只能调用常量成员函数。不能在一个常量对象上调用普通的成员函数。

  - **引入常量成员函数作用**： 常量成员函数即可以被常量对象调用，也可以被普通对象调用；而普通成员函数只能被普通对象调用，不能被常量对象调用。因此把`this`设置为指向常量的指针有助于提高函数灵活性。

    ```c++
     #include<iostream>
      using std::cin;
      using std::cout;
      using std::endl;
      int main(){
      	class test{
      	public:
      		int func1(){return 11;}
      		int func2() const {return 12;};	
      	};
      	test A;
      	cout << A.func1() << endl;
      	cout << A.func2() << endl;

      	const test B;
      	//cout << B.func1() << endl; 
          // 会报错如下：
        	/* error: passing ‘const main()::test’ as ‘this’ argument discards   	  qualifiers [-fpermissive]
        cout << B.func1() << endl;                  ^
        即上文所说：在默认情况下this的类型是指向类类型非常量版本的常量指针，因此不能将默认的this绑定到一个常量对象上，也就使得我们不能在一个常量对象上调用普通的成员函数。
      */
      	cout << B.func2() << endl;

      	return 0;
      }
    ```


- 类作用域和成员函数

  - 类本身就是一个作用域。
  - 当在类的外部定义成员函数时，成员函数的定义必须与它的声明匹配。

#### 7.1.3 定义类相关的非成员函数

- 有些接口函数的操作从概念上来说属于类的接口的组成部分，但他们实际上不属于类本身。一般来说这些属于接口部分的非成员函数的声明要和类放在同一个头文件内。

  ```c++
  istream &read(istream &is, Sales_data &item)
  {
    double price = 0;
    is >> item.bookNo >> item.units_sold >> price;
    item.revenue = price * item.units_sold;
    return is;
  }
  ostream &print(ostream &os, const Sales_data &item)
  {
    os << item.isbn() << " " << item.units_sold << " "
       << item.revenue << " "  << item.avg_price();
    return os;
   }
  ```

  ​

#### 7.1.4 构造函数

- 构造函数：类通过一个或几个特殊的成员函数来控制其对象的初始化过程，这些函数叫构造函数。无论何时只要类的对象被创建，就会执行构造函数。**构造函数的任务**是初始化类对象的数据成员。

- 构造函数没有返回类型。

- 默认构造函数：当没有人为的显示的定义构造函数时，编译器会隐式地定义一个默认构造函数。

- 编译器构造的函数又被称为**合成的默认构造函数**，该函数将按照如下规则初始化类的数据成员:

  - 如果存在类内初始值，用它来初始化成员。
  - 否则，默认初始化该成员。

- 对于一个普通的且含有其他构造函数的类必须定义自己的默认构造函数。原因如下：

  - 编译器只有在发现类不包含任何构造函数时才会自动生成默认构造函数。一旦类定义了其他构造函数，那么除非再定义一个默认的构造函数，否则该类将不存在默认构造函数。
  - 对于某些类来说，合成默认函数可能会执行错误的操作。如果类内包含内置类型或复合类型的成员，则只有当这些成员全被赋予了类内的初始值时，这个值才适用于合成的默认构造函数。
  - 有时候编译器不能为某些类合成默认的构造函数。例如一个类中包含一个其他类类型的成员且这个成员的类型没有默认构造函数，那么编译器将无法初始化该成员。

- 定义`Sales_data` 的构造函数：

  ```c++
  struct Sales_data{
    Sales_data() = default;
    Sales_data(const std::string &s):bookNo(s){ }
    Sales_data(const std::string &s, unsigned n, double p):bookNo(s), units_sold(n), revenue(p*n){ }
    Sales_data(std::istream &);
    // 以下为原来已有的其他成员
  }
  ```

  - 其中`Sales_data() = default;`不接受任何参数，因此它是一个默认构造函数。定义这个函数的目的是因为这里既需要其他形式的构造函数，也需要默认的构造函数；当有其他构造函数存在时，如果不显示定义默认构造函数，编译器是不会隐式定义默认构造函数的。
  - 在上述代码中第二个和第三个构造函数的冒号和花括号之间的部分被称为**构造函数初值列表**，其作用是为新创建的对象的一个或几个数据成员赋初值。这两个函数的函数体是空的，因为构造函数的唯一目的就是为数据成员赋初值。
  - **定义在类外部的构造函数**： 第四个构造函数与第二个和第三个构造函数不同，它是以`istream`为参数的构造函数，所以需要执行一些实际操作，在它的函数体内调用了`read`函数。虽然这个构造函数没有初值列表（初值列表是空的），但执行过函数体后，对象的成员仍然能被初始化。

  ```c++
  Sales_data::Sales_data(std::istream &is){
    read(is, *this);
  }
  // 这里先把 Sales_data::Sales_data 看成一个在类内声明，在类外实现的成员函数，即定义了一个 Sales_data 类的成员，它的名字是Sales_data。因为该成员函数的名字和类的名字相同，所以它是一个构造函数。
  ```


### 7.2 访问控制与封装

- 在C++ 中使用访问说明符加强类的封装：

  - 定义在`public` 说明符后的成员在整个程序内可以被访问，`public`成员定义了类的接口。
  - 定义在`private`说明符后的成员可以被类的成员函数访问，但不能被使用该类的代码访问，`private`部分封装了（即隐藏了）类的实现细节。

- 新版`Sales_data`类:

  ```c++
  class  Sales_data{
    public:
    	Sales_data() = default;
    	Sales_data(const std::string &s):bookNo(s){ }
    	Sales_data(const std::string &s, unsigned n, double p):bookNo(s), units_sold(n), revenue(p*n){ }
    	Sales_data(std::istream &);
    	std::string isbn() const { return bookNo; }	
    	Sales_data &combins(const Sales_data); 		
    private:
        double avg_price() const{return units_sold ? revenue/units_sold : 0;}     
    	// 数据成员
    	std::string bookNo;
    	unsigned units_sold = 0;
    	double revenue = 0.0;
  }
  ```

- **`class`关键字：**这里用`class`代替了原来的`struct`。其实这两个关键字的**唯一区别**就是默认的访问权限不一样。`class`的默认访问权限是`private`，`struct`的默认访问权限是`public`。

#### 7.2.1 友元

- 当有类外非成员接口函数要操作类内`private`权限的数据成员时，要通过`friend`关键字将这些函数声明成友元：

  ```c++
  class  Sales_data{
    friend Sales_data add(const Sales_data&, const Sales_data&);
    friend std::istream &read(std::istream&, Sales_data&);
    friend std::ostream &print(std::ostream&, const Sales_data&);
    public:
    	Sales_data() = default;
    	Sales_data(const std::string &s):bookNo(s){ }
    	Sales_data(const std::string &s, unsigned n, double p):bookNo(s), units_sold(n), revenue(p*n){ }
    	Sales_data(std::istream &);
    	std::string isbn() const { return bookNo; }	
    	Sales_data &combins(const Sales_data); 		
    private:
  	double avg_price() const{return units_sold ? revenue/units_sold : 0;}     
    	// 数据成员
    	std::string bookNo;
    	unsigned units_sold = 0;
    	double revenue = 0.0;
  }
  Sales_data add(const Sales_data&, const Sales_data&);
  std::istream &read(std::istream&, Sales_data&);
  ```

- 友元不是类的成员函数，所以不受它所在的类内的位置区域的访问控制级别约束。

- 友元在类内的声明仅仅指定了访问的权限，不是通常意义上的函数声明，所以如果用到某个函数时应在类外专门再对函数声明一次。

- **封装的好处**：

  - 确保用户代码不会无意间破坏对象的状态。
  - 被封装的类的具体实现细节可以随时改变，而无须调整用户级别的代码。

### 7.3 类的其他特性
#### 7.3.1 类成员再探
- **令成员作为内联函数：**
    - 定义在类内的成员函数是自动`inline`的，即隐式内联。
    - 也可以在类内声明时用`inline`关键字显示内联，然后在类外进行成员函数的定义。
    - 也可以不在类内显示声明，而是在类外进行成员函数定义是用`inline`关键字显示修饰。

    ```c++
     // Screen 类成员函数举例
      class Screen{
      public:
          typedef string::size_type pos;
          Screen() = default;
          Screen(pos ht, pos wd, char c): height(ht), width(wd), contents(ht*wd, c) { }
          char get(){ return contents[cursor];} // 读取光标位置，隐式内联
          inline char get(pos ht, pos wd);      // 类内显示内联
          Screen &move(pos r, pos c);          // 类外显示内联，会在类外定义函数时会用`inline`关键字修饰
      private:
          pos heigth = 0;
          pos width = 0;
          pos cursor = 0;
          std::dtring cpntents;
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
    ```



- **可变数据成员：** 

  - 如果希望能修改某个数据成员，即使在一个`const`成员函数内（上文说道常量成员函数只能读取调用它的对象的数据成员，但不能写入新值），可以通过在变量的声明中加入`mutable`关键字来做到这一点。
  - 一个可变数据成员永远不会是`const`，即使它是`const`对象的成员。

- 类数据成员的初始值：

  - 如果一个类中某个成员是其他类类型的，那么在初始化这种‘类类型’的成员时需要构造函数传递一个符合成员类型的实参。

  ```c++
  class Window_mgr{
    private:
    	// 默认情况下，一个Window_mgr包含一个标准尺寸的空白Screen
    	std::vector<Screen> screens{Screen(24, 80, ' ')};
  }
  ```

  - 当提供类内初始值时，必须使用`=`或花括号`{}`表示。

#### 7.3.2 返回`*this`的成员函数

