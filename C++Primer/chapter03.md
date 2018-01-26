## 第三章 字符串、向量和数组

### 3.1 命名空间using的声明

- 操作符 :: 的意思是：编译器应从操作符左侧名字所示的作用域中寻找右侧那个名字。
- std::cin 的意思是使用命名空间 std 中的名字 cin 。
- 上面这种方法比较麻烦，每次要使用 cin 的时候都要使用 std:: ，因此可以使用 using 声明
- 格式：`using namespace::name;`，例如：`using std::cin;`。一旦这样声明了 cin，则之后再使用cin 就不需要每次都加专门的`std::`前缀。
- 每个using声明引入命名空间的一个成员。
- 头文件不应包含 using 声明，因为头文件的内容会被拷贝到所有引用这个头文件的其他文件中，这样可能会对其他文件内容产生冲突。

### 3.2 标准库类型 string

- 范围 for 循环

  ```c++
  std::string s = "hello world!!!";
  for(auto &c: s)
    c = toupper(c);
  std::cout << s << std::endl;
  ```

  ​

### 3.3 标准库类型 vector

- vector 表示对象的集合，其中所有对象的类型相同。
- 引用不是对象，所以不存在包含引用的vector。

#### 3.3.1 定义和初始化 vector 对象

- 列表初始化 

  ``` c++
  vector<string> v1{"a", "an", "the"};
  ```

- 创建指定数量的元素

  ```c++
  vector<int> ivec(10, -1);  // 10 个 int 类型的元素，每个都被初始化为 -1
  vector<string> svec(10, "hi"); // 10 个 string 类型的元素，每个都被初始化为 "hi"
  ```

- 值初始化

  ```c++
  vector<int> ivec(10); // 10 个元素，每个都初始化为 0
  vector<string> svec(10) // 10 个元素，每个都被初始化为空 string 对象
  ```


- 一般来说用圆括号()，可以说提供的值是用来构造vector对象的，而花括号{}是用来列表初始化vector对象的，但也有特例：如果使用了花括号的形式，但括号里提供的值又不能用来列表初始化，那就要考虑是用这样的值来构造vector对象了。

  ```c++
  // 如果列表初始化一个含有string对象的vector对象，应该提供能赋值给string对象的初值。
  vector<string> v1{10}; //v1 有10个默认初始化的值
  vector<string> v2{10, "hi"}; // v2 有10个值为”hi“的元素
  vector<string> v3(10, "hi"); // 这才是常规的构造vector的表达
  ```

  ​

#### 3.3.2 向 vector 对象中添加元素

- 用 vector 的成员函数 push_back 向一个事先创建好的 vector 对象中添加元素，添加到 vector 的尾端。

  ```c++
  vector<int> v;
  for(int i = 0; i <= 100; i++)
  	v.push_back(i);
  ```

  ​







