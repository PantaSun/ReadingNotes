## 第四章 表达式

### 4.1 基础

#### 4.1.1 基本概念

- 重载运算符：C++语言定义了运算符作用于内置类型和复合类型的运算对象时所执行的操作。当运算符作用于类类型的运算对象时，用户可以自定义其含义。因为这种自定义的过程事实上是为已存在的运算符赋予了另外一层含义。IO库的`>>`和`<<`运算符以及`vector`对象、`string`对象和迭代器使用的运算符都是重载运算符。
- 左值和右值：这里有点蒙蔽。
  - 当一个对象被用作右值时，用的是对象的值（内容）；当对象被当做左值时，用的是对象的身份（在内存中的位置）。

#### 4.1.3 求值顺序

- 有四种运算符明确规定了运算对象的求值顺序。分别是：`&&`、`||`、 `?:` 和 `,` 。
- 处理复合表达式时，两条建议
  - 拿不准的时候最好用括号来强制让表达式的组合关系复合逻辑要求。
  - 如果改变了某个运算对象的值，在表达式的其他地方不要在使用这个运算对象。

### 4.2 算数运算符

- 一元运算符的优先级最高，其次是乘法和除法，优先级最低的是加法和减法。
- 算数运算符的运算对象和求值结果都是右值。
- 在表达式求值之前，小整数类型的运算对象被提升为较大的整数类型，所有运算对象最终会转换成同一类型。
- 参与取余运算的对象必须是整数类型。
- C++11 新标准规定商一律向0取整。
- `(-m) / n`和`m / (-n)` 都等于`-(m / n)`。而`m %(-n)`等于`m % n`，`(-m) % n`等于`-(m % n)`。

### 4.3 逻辑运算符

- 短路求值：
  - 逻辑与：当且仅当左侧运算对象为真时才对右侧运算对象求值。
  - 逻辑或：当且仅当左侧运算对象为假时才对右侧运算对象求值。

### 4.4 赋值运算符

- 赋值运算符的左侧运算对象必须是一个可修改的左值。
- 赋值运算符满足右结合律：`int a, b; a = b = 0;`因为满足右结合律，所以`b = 0`作为左边赋值运算符的右侧运算对象，即`a = (b = 0);`，又因为赋值运算符返回的是左侧运算对象，所以`b = 0`返回的是`b`，然后再执行`a = b`，最后`a`和`b`都被赋值为`0`。


### 4.5 递增和递减运算符

- 除非必须，否则不用递增或递减运算符的后置版本。

- `*pbeg++` 输出当前值，并将`pbeg`向前移动一个元素。

- `++*pbeg`先将`pbeg`向前移动一个元素，再输出该元素的值。

- 避免产生未定义错误：

  ```c++
  while(beg != s.begin() && !isspace(*beg))
  	*beg = toupper(*beg++);
  ```

  问题在于：赋值运算符左右两端都用到了运算对象beg，并且右侧的运算对象还改变了beg的值，所以该赋值语句是未定义的。因为这可能有两种处理顺序：

  ```c++
  // 第一种 先求左侧的值
  *beg = toupper(*beg);
  ++beg;
  // 第二种 先求右侧的值
  *(beg + 1) = toupper(*beg); 
  ```

### 4.6 成员访问运算符

- `ptr->mem`等价于`(*ptr).mem`。
- 解引用运算符的优先级低于点运算符`*p.size()`是一种错误表达，先运行`p`的`size`，再对该结果解引用，但是`p`是一个指针，没有名为`size`的成员。

### 4.9 `sizeof`运算符

- 满足右结合律。
- 返回结果是`size_t`类型的常量表达式。
- 有两种形式：
  - `sizeof (type)`
  - `sizeof expr`:返回表达式结果类型的大小
- `siezof` 并不实际计算其运算对象的值。
- C++11 新标准允许我们使用作用域运算符获取类成员大小。通常情况下只有通过类的对象才能访问到类的成员，但是`sizeof`运算符不需要我们提供一个具体的对象，因为要想知道类成员的大小，不用真的获取该成员。
- `char` 或这类型为`char`的表达式执行`sizeof` 时，结果为`1`。
- 对引用类型执行`sizeof`运算得到被引用对象所占空间大小。
- 对指针执行`sizeof`运算得到指针本身占空间的大小。
- 对解引用指针执行`sizeof`运算得到指针指向的对象所占空间的大小，指针不需要有效。
- 对数组执行`sizeof`运算得到整个数组所占空间的大小，相当于将数组中每一个元素各执行一次`siezof`并将结果求和。这里`sizeof`运算不会将数组转换成指针来处理。
- 对`string`和`vector`执行`sizeof`运算只返回该类型固定部分的大小，不会计算对象中的元素占用了多少空间。（在我电脑上`string`返回`32`；`vector`返回`24`）。

### 4.10 逗号运算符

- 按照从左向右的顺序依次求值。
- 先对左侧的表达式求值，然后将求值结果丢掉，逗号运算符的真正的结果是右侧表达式的值。

### 4.11 类型转换

#### 4.11.1 隐式转换

- 何时发生隐式类型转换
  - 在大多数表达式中，比`int`小的整形值首先提升为较大的整数类型。
  - 在条件中，非布尔型转换为布尔型。
  - 初始化过程中，初始值转化为变量的类型：在赋值语句中，右侧运算对象转换成左侧运算对象的类型。
  - 算术运算或关系运算的运算对象有多种类型，需要转换成同一种类型。
  - 函数调用时也会发生类型转换。
- 无符号类型的转换结果要依赖于机器中各个整数类型的相对大小。

#### 4.11.3 显式转换

- `cast-name<type>(expression);`，`type`是转换的目标类型；`expression` 是要转换的值；`cast-name`有四种：
  - `static_cast`：除了包含底层`const`，都可以使用。
  - `dynamic_cast`：支持运行时类别识别。
  - `const_cast`：只能改变运算对象的底层`const` 。
  - `reinterpret_cast`：通常为运算对象的位模式提供较低层次上的重新解释。
- 尽量避免强制类型转换，因为其干扰了正常的类型检查，这容易发生错误。
