## 第十章 泛型算法

- 标准库定义了一组泛型算法，实现了一些经典算法的借口，因此也称他们为“算法”。
- 大多数算法定义在头文件algorithm中，标准库还在头文件numeric中定义了一组泛型算法。
- 一般情况下，这些算法不直接操作容器，而是遍历由两个迭代器指定的一个元素范围来进行操作。
- 泛型算法永远不会改变底层容器的大小。虽然可能改变容器中保存的元素的值，也可能在容器内移动元素，但永远不会直接添加或删除元素。

### 初识泛型算法

- 向目的位置迭代器写入数据的算法假定目的位置足够大，能容纳要写入的元素。

#### 介绍back_inserter（定义在头文件iterator中）

- 一种保证算法有足够元素空间来容纳输出数据的方法是使用插入迭代器（insert iterator）

- 插入迭代器是一种向容器添加元素的迭代器。

- back_inserter接受一个指向容器的引用，返回一个与该容器绑定的插入迭代器。当通过此迭代器赋值是，赋值运算符会调用push_back将一个具有给定值的元素添加到容器中：

  ```c++
  vector<int> vec; //空向量
  auto it = back_inserter(vec); //通过它赋值会将元素添加到vec中
  *it = 42; //vec中现在有一个元素，值为42
  ```

### 定制操作

#### lambda表达式

- 一个lambda表达式表示一个可调用的代码单元。

- lambda具有一个返回类型、一个参数列表和一个函数体。与函数不同的是，lambda可能定义在函数内部。

  ```c++
  [capture list](paramter list) -> return type {function body}
  ```

  其中捕获列表（capture list）是一个lambda所在函数中定义的局部变量列表；return type、parameter list和function body 与任何普通函数一样。

- 可以忽略参数列表和返回类型，但必须用韵包含捕获列表和函数体。

#### lambda捕获和返回

- 当向一个函数传递lambda时，同时定义了一个新类型和该类型的一个对象：传递的参数就是此编译器生成的类类型的未命名对象。
- 当使用auto定义一个用lambda初始化的变量时，定义了一个从lambda生成的类型的对象。

#####  捕获值

- 引用捕获

  - 若采用引用捕获一个变量，就必须保证被引用对象在lambda执行的时候时存在的。
  - lambda捕获的都是局部变量，这些变量在函数结束后就不复存在了。

- 隐式捕获

  - 可以在捕获列表中写一个&或=。

  - &告诉编译器采用引用捕获方式

  - =告诉编译器采用值捕获方式

  - 也可以混合捕获

    ```c++
    // os隐式捕获，引用捕获方式；c显示捕获，值捕获方式。
    for_each(b, e, [&, c](const string &s){os << s << c;});
    ```

  - 当使用混合捕获时，捕获列表第一个元素必须时一个&或=。而且显示捕获的变量必须与隐式捕获不同的方式。即如果隐式是&，则显示必须时值捕获方式；若隐式是=，则显示必须时引用捕获方式。

##### 可变lambda

- 默认情况下，对于一个值被拷贝的变量，lambda不会改变其值。如过希望改变一个被捕获的变量的值，就必须在参数列表首加上关键字mutable。因此可变lambda可以省略参数列表：

  ```c++
  void fun(){
      int v1 = 42; //局部变量
      // f可以改变它所捕获的变量的值
      auto f = [v1]()mutable{return ++vl;};
      v1 = 0;
      auto j = f(); // j为43
  }
  ```

- 一个引用捕获的变量是否可以修改依赖于此引用指向的是一个const类型还是非const类型。

  ```c++
  void fun1(){
      int v1 = 42;//局部变量
      //v1 是一个非const变量的引用
      //可以通过f2中的引用来改变它
      auto f2 = [&v1]{return ++v1;};
      v1 = 0;
      auto j = f2(); // j为1
  }
  ```

##### 指定lambda返回类型

- 默认情况下，如果一个lambda体包含return之外的任何语句，则编译器假定此lambda发挥void。

  ```c++
  fun(b, e, [](int i){if(i<0) return -i;else return i;}); //报错：不能推断lambda的返回类型。
  ```

- 因此当需要为lambda定义返回类型时，必须使用尾置返回类型：

  ```c++
  fun2(b, e, [](int i)-> int {if(i < 0) return -i; else return i;});
  ```

#### 参数绑定

##### 标准库bind函数

- 定义在头文件functional中。可以将bind函数看作一个通用的函数适配器，它接受一个可调用对象，生成一个新的可调用对象来适应原对象的参数列表。

  ```c++
  // 调用bind的一般形式
  auto newCallable = bind(callable, arg_list);
  ```

  其中newCallable本身是一个可调用对象，arg_list是一个逗号分隔的参数列表，对应给定的callable的参数。即，当我们调用newCallable时，newCallable会调用callable，并传递给它arg_list中的函数。

- arg_list中可能包含形如 \_n的参数，这里的n是一个整数。这些参数是“占位符”，表示newCallable的参数，它们占据了传递给newCallable的参数的位置。例如：_1为newCallable的第一个参数，\_2位第二个参数，以此类推。

- 举例

  ```c++
  // 首先check_size()
  bool check_size(const string &s, string::size_type sz ){
      return s.size() >= sz;
  }
  // check6是一个可调用对象，接受一个string类型的参数
  // 并有此string和值6来调用check_size
  auto check6() = bind(check_size, _1, 6);
  string s = "hello";
  bool b1 = check6(s); // check6会调用check_size(s, 6)
  //当传递到函数中时
  autp wc = find_if(words.begin(), words.end(), bind(check_size, -1, sz));
  // 次bind调用生成一个可调用对象，将check_size的第二个参数绑定到了sz的值。当find_if对words中的
  // string调用这个对象时，这些对象会调用check_size，将给定的string和sz传递给它。
  ```

##### 使用placeholders名字

- \_n都被定义在一个名为palceholders的命名空间中，而这个空间本身定义在std命名空间中。例如，\_1对应的using声明为：`using std::placeholders::_1;`，为了方便起见可以使用：`using namespcae std::palceholders;`，这样就不用为每个占位符都进行声明了。

##### 绑定引用参数

- 当希望传递给bind一个对象而又不拷贝它，就必须使用标准库ref函数：

  ```c++
  for_each(words.begin(), words.end(), bind(print, ref(os), _1, ' '));
  ```

- 函数ref返回一个对象，包含给定的引用，此对象时可以拷贝的。标准库中还有个cref函数，生成一个保存const引用的类。ref和cref定义在functional头文件中。



