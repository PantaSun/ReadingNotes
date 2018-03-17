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

- 成员函数的声明必须在类的内部，它的定义则既可以在在类的内部，也可以在类的外部。

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

  ​


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

- 如果一个类的多个普通（非`const`）成员函数以引用的形式返回`*this`，那么这几个函数可以连接在一条表达式中：

  ```c++
  Screen myScreen;
  myScreen.move(4, 0).set('#');
  // 上述连接语句等价于
  myScreen.move(4, 0);
  myScreen.set('#');
  // 若返回的类型不是引用，即move成员函数和set成员函数返回Screen而非Screen&，则move的返回值将是*this的副本，上述连接语句等价于
  Screen temp = myScreen.move(4, 0);
  temp.set('#');
  ```


- 从`const`成员函数返回`*this`：一个`const`成员函数如果以引用的形式返回`*this`，那么它的返回类型将是常量引用。

- 基于`const`的重载：通过区分成员函数是否`const`，可以对该成员函数重载。原因是：

  - 非常量版本的成员函数对于常量对象是不可用的，只能在一个常量对象上调用`const`成员函数。
  - 对于非常量对象来说，非常量成员函数是一个更好的匹配。下边做一个测试：

  ```c++
  #include<iostream>
  using std::cin;
  using std::cout;
  using std::endl;
  int main(){
  	class test
  	{
  		public:
  			int func1(){return 11;}
  			int func1() const {return 12;}	
  	};

  	test A;
  	const test B;

  	cout << "A.func1():" << A.func1() << endl;
  	cout << "B.func1():" << B.func1() << endl;
  	
  	return 0;
  }
  ```

  返回结果：

  ```
  panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ g++ test.cpp 
  panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ ./a.out 
  A.func1():11
  B.func1():12
  panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ 

  ```

  附上练习7.27的代码：

  ```c++
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
  int main(){
      /* code */
      Screen myScreen(5, 5, 'X');
      myScreen.move(4, 0).set('#');
      cout << endl;
      myScreen.display(cout);
      cout << endl; 
      return 0;
  }
  /*输出结果：
    panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ g++ test1.cpp 
    panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ ./a.out 

    XXXXXXXXXXXXXXXXXXXX#XXXX
    panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ 
  */
  ```


 

#### 7.3.3 类类型  

- 每个类的类型是唯一的，即使两个类的成员列表完全一致，它们也是不同的类型。
- 类的声明：
  - 前向声明：仅声明类而暂时不定义它。例如`class Screen； `只是向程序里引入`Screen`，并指明`Screen`是一中类类型。而`Screen`在被定义之前是一个不完全类型，即不知道该类包含哪些成员。
  - 不完全类型使用情景：可以定义指向这中类型的指针或引用，也可以声明以不完全类型作为参数或者返回类型的函数。

#### 7.3.4 友元再探

- 友元的定义：

  - 类不仅可以把普通的非成员函数定义成友元，
  - 还可以把其他类定义为友元，
  - 也可以把其他类的成员函数定义成友元。
  - 此外，友元函数能定义在类的内部，这样的函数是隐式内联的。

- 类之间的友元关系：如果A类的某些成员要访问B类的内部数据，则可以把A类指定成B类的友元：

  ```c++
  class Screen{
  	// Window_mgr 的成员可以访问 Screen 类的私有部分
  	friend class Window_gmr;
  }
  ```

  ```c++
  class Window_mgr{
  	public:
  		using ScreenIndex = std::vector<Screen>::size_type;
  		void clear(ScreenIndex);
  	private:
  		std::vector<Screen> screens{Screen(24, 80, ' ')};
  	
  }

  void Window_mgr::clear(ScreenIndex i){
  	Screen &s = screens[i];
  	s.contents = string(s.height * s.width, ' ');
  }
  ```

  - 如果一个类指定了友元类，则友元类的成员函数可以访问**此类包括非公有成员在内的所有成员**。
  - 友元不具有传递性，即如果`Window_mgr`有自己的友元类，那么这些友元并不能理所当然的访问`Screen`类。
  - 每个类负责控制自己的友元类或友元函数。

- 令成员函数作为友元：一个A类可以只为某个B类的某个成员函数提供访问权限，不用向整个B类提供访问权限。

  ```c++
  class Screen{
  	// Window_mgr::clear 必须在Screen类之前被声明
  	friend void Window_mgr::clear(ScreenIndex);
  }
  ```

- 友元声明和作用域

  - 友元的声明影响访问权限，并非普通意义上的声明。


### 7.4 类的作用域

- 一方面，当类的成员函数定义在类外时，必须在函数名之前提供类名。在类的外部，成员的名字被隐藏起来了。这样的话，我们可以在这个类外成员函数的定义中可以直接使用类的其他成员而不用再次授权。

  ```c++
  void Window_mgr::clear(ScreenIndex i){
  	Screen &s = screens[i];
      s.contents = string(s.height * s.width,' ');
  }
  // 这里的ScreenIndex 和 screens都是Window_mgr的成员，所以可以直接使用
  ```

- 另一方面，函数的返回类型通常出现在函数名之前，当成员函数定义在类外部时，返回类型中使用的名字都位于类的外部，这时返回类型就必须指明他是那个类的成员。

  ```c++
  class Window_mar{
  	public:
    		// 向窗口添加一个Screen，并返回它的编号
  		ScreenIndex addScreen(const Screen&);
    	// 其他成员与之前一样
  }

  Window_mgr::ScreenIndex Window_mgr::addScreen(const Screen &s){
  	screens.push_back(s);
  	return screen.size() - 1;
  }
  ```

#### 7.4.1 名字查找与类的作用域

- 编程中的名字查找
  - 先在名字所在块中寻找其声明语句，只考虑在使用前出现的声明
  - 如果没找到继续查找外层作用域
  - 如果最终没找到匹配的声明，则程序报错


- 类内部成员函数名字的解析与上述查找规则有所区别，类的定义分两步处理：

  - 首先，编译成员的声明
  - 直到全部可见后才编译函数体
  - ps：编译器处理完类中的全部声明后才会处理成员函数的定义

- 用于类成员声明的名字查找：

  - 上述的两阶段处理方式只适用于成员函数中使用的名字
  - 关于声明中使用的名字，包括返回类型或则参数列表中使用的名字，都必须在使用前确保可见

  ```c++
  typedef double Money;
  std::string bal;

  class Account{
  	public:
  		Money blance() {return bal;}
    	
    	private:
    		Money bal;
  };
  // 这里当编译器看到 blance 函数的声明时，先在 Account 中查找 Money 的声明，没找到就去外层作用域找，最终找到 Money 的 typedef 语句。而balance函数的返回类型和数据成员bal也都是Money类型的。
  // blance 函数体在整个类被处理后才可见，所以 return 返回的是类中名为 bal 的成员，而不是类外 string 类型的 bal
  ```

- 类型名要特殊处理：类中成员如果使用了外层作用域中的代表某一类型的名字，则类之后不能重新定义该名字。即使该名字的重新定义的类型与外层作用域中所定义的类型一样。

- 成员函数定义中普通块作用域的名字查找：

  - 先在成员函数内查找，即只有在函数使用之前出现的声明才会被考虑。
  - 若成员函数内部没有就在类内继续查找，这时候就要考虑类内所有成员。
  - 如果类内也没有，那就在成员函数定义之前的所用域内继续查找。

- 如果成员函数需要外层作用域的名字，则可以显示的通过作用域运算符`::`来进行请求，如下测试。

  ```c++
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
  /*
  输出如下：
  panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ g++ 7.35.cpp 
  panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ ./a.out 
  initVal()
  setVal()
  10
  panta@panta-PC:/media/panta/0AAF058E0AAF058E/ReadingNotes/C++Primer$ 
  */
  ```

  ​

### 7.5 构造函数再探

#### 7.5.1 构造函数初始值列表

- 如果成员是`const`或是引用的话，就必须将其初始化；同理，如果成员属于某种类类型且该类没有默认构造函数时，也必须将这个成员初始化。而且必须通过构造函数初始值列表来为这些成员初始化。
- 我们初始化`const`或引用类型的数据成员的唯一机会就是通过构造函数初始值。
- 成员初始化的顺序与它们在类定义中出现的顺序一致。
- 一般来说成员初始化的顺序没有特别的要求，但是如果成员`a`的是用成员`b`来初始化的，那么就需要成员`b`先于成员`a`进行初始化。
- c++11 新标准可以定义委托构造函数：一个构造函数把它自己的一些（或全部）职责委托给了其他构造函数。

#### 7.5.4 隐式的类类型转换

- 如果构造函数只接受一个实参，则它实际上定义了转换为此类类型的隐式转换机制。

- 能通过一个实参调用的构造函数定义了一条从构造函数的参数类型向类类型隐式转换的规则。

  ```c++
  string null_book = "9-999-99999-9";
  item.combine(null_book);
  ```

  这里`item`是一个`Sales_data`对象，这种调用是合法的，编译器用给定的`string`自动创建一个`Sales_data`对象。

- 只允许一步类类型转换，如下代码是错误的，因为它执行了两步隐式转换：先将`"9-999-99999-9"`转换成`string`，再将`string`转换成`Sales_data`：

  ```c++
  item.combine("9-999-99999-9");
  ```

- 抑制构造函数的隐式转换：可以通过将构造函数声明为`explicit`来阻止隐式转换。这个关键字只对一个实参的构造函数有用，因为多个实参的构造函数本身就不能用于执行隐式转换。

- 当用`explicit`关键字来声明构造函数时，它将只能以直接初始化的形式使用，而不能用于拷贝形式的初始化。

  ```c++
  Sales_data item1(null_book);  // 正确
  Sales_data item2 = null_book; // 错误，不能用于拷贝形式的初始化
  ```

#### 7.5.5 聚合类

- 聚合类使用户可以直接访问其成员，并有特殊的初始化语法：

  - 所有成员都是`public`
  - 没有定义任何构造函数
  - 没有类内初始值
  - 没有基类，也没有`virtual`函数

  ```c++
  // 一个聚合类
  struct Data{
    int ival;
    string s;
  }
  // 初始化聚合类，注意初始值的顺序要与声明的顺序一致
  Data val1 = {0, "Anna"};
  ```

  ​

### 7.6 类的静态成员

- 声明静态成员：

  ```c++
  class Account{
  	public:
    		void calculate() {amount += amout * interestRate;}
    		static double rate() {return interestRate;}
    		static void rate(double);
      private:
    		std::string owner;
    		double amount;
    		static double interestRate;
    		static double initRate();
  };
  void Account::rate(double newRate){
    interestRate = newRate;
  }
  ```

  ​

  - 在成员的声明前加上`static`关键字，这样就能使该成员与类关联在一起，而不与类的各个对象保持关联（和‘与类关联’相对应的是与类的各个对象关联）。
  - 类的静态成员存在于任何对象之外，对象中不包含任何与静态数据成员有关的数据。
  - 静态成员也不和任何对象绑定到一起，它们不包含`this`指针。
  - 静态成员不能声明成`const`的。
  - 不能在`static`成员函数体内使用`this`指针。

- 使用类的静态成员：

  - 使用作用域运算符直接访问静态成员`double r = Account::rate();`
  - 虽然静态成员不属于类的某个对象，但仍可以使用类的对象、引用和指针来访问静态成员：

  ```c++
  Account act1;
  Account act2* = &act1;

  r1 = act1.rate();
  r2 = act2->rate();
  ```

  - 类中成员成员函数就不用作用域运算符直接就能使用静态成员。

- 定义静态成员：

  - 和其他成员函数一样，静态成员函数既可以在类内定义也可以在类外定义。
  - 静态成员不是由类的构造函数初始化的，因为他们并不是在类创建对象时被定义的。
  - 必须在类的外部定义和初始化每个静态成员。且一个静态数据成员只能定义一次。

  ```c++
  double Account::interestRate = initRate();
  ```

  - 上述代码中，从类名开始这条语句的剩下部分就都位于类的作用域内了，因此这里可以使用类的私有函数`initRate()`。