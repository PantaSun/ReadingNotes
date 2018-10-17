## 第十二章 动态内存

- 除了自动和static对象外，C++还支持动态分配对象。
- 动态分配对象的生存期与它们在哪里创建无关，只是当显示地被释放时，这些对象才会销毁。
- 为了更安全的使用动态对象，标准库定义了两个智能指针类型来管理动态分配的对象。
- 当一个对象应该被释放时，指向它的智能指针可以确保自动的释放它。
- 静态内存和栈内存：分配在静态内存或栈内存的对象由编译器自动创建和销毁
  - 静态内存用来保存局部static对象、类static数据成员以及定义在任何函数之外的变量。
  - 栈内存用来保存定义在函数内的非static对象。
- 除了静态内存和栈内存，每个程序还拥有一个内存池，这部分内存被称为自由空间或堆。
- 程序用堆来存储动态分配的对象，即那些在程序运行时分配的对象。

### 动态内存与智能指针

- C++中动态内存是通过一对运算符来完成的：

  - new：在动态内存中为对象分配空间并返回一个指向该对象的指针，可以选择对对象初始化。
  - delete：接受一个动态对象的指针，销毁该对象，并释放与之关联的内存。

- 为了更容易的使用动态内存，新标准库提供了两种智能指针来管理对象。

  - shared_ptr：允许多个指针指向同一个对象
  - unique_ptr：独占所有指向的对象
  - weak_ptr：它是一种若引用，指向share_ptr所管理的对象。
  - 这三种类型都定义在memory头文件中。

- 智能指针与普通指针的区别是它负责自动释放所指向的对象。

  

#### share_ptr类

- 智能指针也是模版，所以在创建时需要提供指针可以指向的类型。

  ```c++
  shared_ptr<string> p1; // 可以指向string
  shared_ptr<list<int>> p2; // 可以指向int的list
  ```

- 默认初始化的智能指针中保存着一个空指针。

- make_shared函数

  ```c++
  shared_ptr<int> p3 = make_shared<int>(42); // 指向一个值为43的int的shared_ptr
  shared_ptr<string> p4 = make_shared<string>(10, '9'); // 指向一个值为“9999999999”的string
  shared_ptr<int> p5 = make_shared<int> (); //指向一个初始化的int，即值为0
  ```

##### shared_ptr的拷贝和赋值

- 当进行拷贝或赋值操作时，每个shared_ptr都会记录有多少个其他的shared_ptr指向相同的对象：

  ```c++
  auto p = make_shared<int>(42); //p指向的对象只有p这一个引用者
  autp q = p; //p和q指向相同的对象，此对象有两个引用
  ```

- 每个shared_ptr都有一个关联的计数器，通常称为引用计数。

- 无论何时拷贝一个shared_ptr，计数器都会递增。

- 当给一个shared_ptr赋一个新值或是shared_ptr被销毁，计数器就会递减。

- 当一个shared_ptr的计数器变为0，它就会自动释放自己所管理的对象。

- 对于一块内存，shared_ptr类保证只要有任何shared_ptr对象引用它，它就不会被释放掉。

- 如果将shared_ptr存放在一个容器中，而后不再需要全部元素，而只适用其中的一部分，要记得用erase删除不再需要的元素。

##### 使用了动态生存期的资源的类

- 程序使用动态内存处于以下三个原因之一：
  - 程序不知道自己需要使用多少对象：容器类就是出于这个原因而使用动态内存的一个典型的例子。
  - 程序不知道所需对象的准确类型
  - 程序需要在多个对象间共享数据



#### 直接管理内存

##### 使用new动态分配和初始化对象

- 在自由空间分配的内存是无名的，因此new无法为其分配的对象命名，而是返回一个指向该对象的指针。

  ```c++
  int *pi = new int; // pi是一个指向动态分配的、未初始化的无名对象
  ```

- 默认情况下，动态分配的对象是默认初始化的，这意味着内置类型或组合类型的对象的值将是未定义的，而类类型对象将用默认构造函数进行初始化。

- 也可以对动态分配的对象进行值初始化，只需要在类型名之后跟一对小括号()即可：

  ```c++
  string p* = new string();
  int *p2 = new int();
  ```

- 对于定义了自己的构造函数的类类型，要求值初始化是没有意义的；不管采用什么形式，对象都会通过默认构造函数来初始化。

- 对于内置类型来说，这两种形式就差别大了：

  - 值初始化的内置类型对象有着良好定义的值。
  - 默认初始化的对象的值则是未定义的。

##### 动态分配的const对象

- `const int *p = new const int(1234);`一个动态分配的cosnt对象必须进行初始化。
- `const string *p2 = new const string;`但是对于一个定义了默认构造函数的类类型，其const动态对象可以隐式初始化，而其他类型的对象就必须显示初始化。
- 由于分配的对象是const的，new返回的指针是一个指向const的指针。

##### 内存耗尽

- 默认情况下，如果new不能分配所要求的内存空间，它会抛出一个类型为bad_alloc的异常。
- 可以使用nothrow来阻止抛出异常：`int *p = new(nothrow)int;`如果分配失败，new返回一个空指针。

##### 释放动态内存

- `delete p;`p必须指向一个动态分配的对象或空指针。
- 虽然一个const对象的值是不能被改变，但它本身是可以被销毁的。

##### 动态内存的管理常见错误

- 忘记delete内存。
- 使用已经释放掉的对象。
- 同一块内存释放两次。

##### delete之后重置指针值

- 在delete之后，指针就变成了空悬指针，指向一块曾经保存过数据对象但现在已经无效的内存的指针。
- 避免空悬指针是的方法：在指针即将离开其作用域之前释放掉它所关联的内存。
- 如果需要保留指针，可以在delete之后将nullptr赋予指针，这样就清楚地指出指针不指向任何对象。

#### shared_ptr和new结合使用

- 不能将一个内置指针隐式转换为一个智能指针，必须使用直接初始化形式来初始化一个智能指针：

  ```c++
  shared_ptr<int> p1 = new int(42); //错误：必须使用直接初始化
  shared_ptr<int> p2(new int(42)); //正确：使用了直接初始化
  ```

- 同理，一个返回shared_ptr的函数不能在其返回语句中隐式转换一个普通指针：

  ```c++
  shared_ptr<int> clone(int p){
      return new int(p); //错误：隐式转换为shared_ptr<int>
  }
  shared_ptr<int> clone1(int p){
      return shared_ptr<int>(new int(p)); //正确：显式的用int*创建shared_ptr<int>
  }
  ```

#### 智能指针和异常

- 当函数因为发生异常未被捕获而退出时，由一个shared_ptr唯一指针指向的内存会被释放掉；而直接管理内存不会被释放，即便函数末尾有delete语句：

  ```c++
  void f(){
      int *ip = new int(42);
      //发生异常未被捕获
      delete ip; 
  }
  ```

  如上述代码所示，假设在new和delete之间发生异常，且异常未在f中被捕获，则内存永远不会释放；而智能指针会通过检查引用计数，当引用计数为0时就会自动释放内存。

##### 智能指针陷阱

- 不使用相同的内置指针初始化或reset多个智能指针
- 不delete get()返回的指针。
- 不使用get()初始化或reset另一个智能指针。
- 如果你使用get()返回的指针，记住最后一个对应的智能指针销毁后，你的指针就变为无效了。
- 如果你使用智能指针管理的资源不是new分配的内存，记住传递给它一个删除器。



#### unique_ptr

- 与shared_ptr不同的是，某个时刻只能有一个unique_ptr指向一个指定对象。

- 当unique_ptr被销毁时，它所指向的对象也被销毁。

- 初始化unique_ptr必须采用直接初始化形式。

- unique_ptr不支持拷贝，也不支持赋值。

- 虽然不能拷贝和赋值，unique_ptr可以通过调用release或reset将指针的所有权从一个非const的unique_ptr转移给另一个unique_ptr：

  ```c++
  unique_ptr<string> p1(new string("hello world!")); //直接初始化
  unique_ptr<string> p2(p1.release()); //将所有权从p1（指向string hello world！）转移给p2
  unique_ptr<string> p3(new string("nihao!")); //直接初始化
  p2.reset(p3.release()); // reset释放了p2原来指向的内存，
  						// 将指向string nihao！的所有权从p3转移给p2
  ```

- unique_ptr的一些操作

  - unique_ptr<T> u1; 空unique_ptr，可以指向类型为T的对象。u1会使用delete来释放它的指针；
  - unique_ptr<T, D> u2; 同上空unique_ptr，u2会使用一个类型为D的可调用对象来释放它的指针。
  - unique_ptr<T, D> u(d); 空unique_ptr，指向类型为T的对象，用类型为D的对象d来代替delete。
  - u = nullptr; 释放u指向的对象，将u置为空。
  - u.release(); u放弃对指针的控制权，返回指针，并将u置空。即，返回unique_ptr当前保存的指针并将其置空。
  - u.reset(); 释放u指向的对象，并将u置空。
  - u.reset(q); 释放u指向的对象，然后令u指向q这个内置指针对象。
  - u.reset(nullptr);释放u指向的对象，并将u置空。

- 若用一个内置指针来保存release返回的指针，则需要手动释放内存，即delete。

##### 传递unique_ptr参数和返回unique_ptr

- 不能拷贝unique_ptr的规则有一个例外：可以拷贝或赋值一个将要被销毁的unique_ptr：

  ```c++
  unique_ptr<int> clone(int p){
      return unique_ptr<int>(new int(p)); //从int*创建一个unique_ptr<int>
  }
  // 还可以返回一个局部对象的拷贝
  unique_ptr<int> clone1(int p){
      unique_ptr<int> ret(new int(p));
      return ret;
  }
  ```

  

 #### weak_ptr

- 是一种不控制所指向对象生存期的智能指针，它指向由一个shared_ptr管理的对象。
  - weak_ptr<T>  w ; 空weak_ptr可以指向类型为T的对象
  - weak_ptr<T>  w(sp); 与shared_ptr sp指向相同对象的weak_ptr，T必须能转换为sp指向的类型。
  - w = p; p可以是一个shared_ptr或weak_ptr。赋值后w与p共享对象。
  - w.reset(); 将w置为空
  - w.use_count(); 与w共享对象的shared_ptr的数量。
  - w.expired(); 若use_count()为0，返回true，否则返回false
  - w.lock();如果expired为true，返回一个空shared_ptr；否则返回一个指向w的对象的shared_ptr。 
- weak_ptr绑定到一个shared_ptr不会改变shared_ptr的引用计数，所以当最后一个指向对象的shared_ptr被销毁，对象就会被释放，即使有weak_ptr指向对象。
- 由于对象可能不存在，所以不能直接使用weak_ptr访问对象，而必须调用lock：此函数检查weak_ptr指向的对象是否存在，如果存在就返回一个指向共享对象的shared_ptr；不存在的话就返回一个空shared_ptr。

###动态数组

#### new和数组

- 用new分配一个对象数组，需要在类型名之后跟一对方括号，在其中指明要分配的对象的数目。

  ```c++
  int *pia = new int[get_size()]; // get_size()返回要分配int的个数，这里用整数也可以的
  ```

- 分配一个数组会得到一个元素类型的指针：当有那个new T[]分配一个数组时，并未得到一个数组类型的对象，而是得到一个数组元素类型的指针。

- 因为分配的内存不是一个数组类型，因此不能对动态数组调用begin和end。也不能使用范围for循环。

##### 动态分配一个空数组是合法的

- 虽然不能创建一个大小为0的静态数组对象，但当n等于0时，调用new[n]是合法的：

  ```c++
  char arr[0];  //错误：不能定义长度为0的数组
  char *p = new char[0]; // 合法：但p不能被解引用
  ```

- new返回一个合法的非空指针。

##### 释放动态数组

- 为了释放动态数组，可以使用delete，并在指针前加上一个空方括号[]：`delete []p;`
- 数组中元素按逆序销毁。
- 当释放一个指向数组的指针时，空方括号是必需的。

##### 智能指针和动态数组

- 标准库提供了可以管理new分配的数组的unique_ptr版本。

  ```c++
  unique_ptr<int[]> up(new int[10]);
  up.release();//自动调用delete[]销毁其指针
  ```

- 当一个unique_ptr指向一个数组时，可以使用下标运算符来访问数组中的元素。

- 指向数组的unique_ptr不支持成员访问运算符。

#### allocator类

- 定义在memory中，帮助我们将内存分配和对象构造分离开来。

- 它分配的内粗是原始的、未构造的。可以按需在此内存中构造对象。

- construct成员函数接受一个指针和零个或多个额外参数，在给定的位置构造一个元素。

  ```c++
  allocator<string> alloc; //可以分配string的allocator对象
  auto const p = alloc.allocate(n); //分配n个未初始化的string
  auto q = p; // q指向最后构造的元素之后的位置
  alloc.construct(q++); //*q为空字符串
  alloc.construct(q++, 10, 'c'); // *q为cccccccccc
  alloc.construct(q++, "hi"); //*q为hi
  ```

- 还未构造对象的情况下使用原始内存是错误的。

- 只能对真正构造了的元素进行destroy操作：`a.destroy(p);`表示对p指向的对象执行析构函数，这里p为*T类型的指针。





