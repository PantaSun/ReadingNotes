## 第五章 语句

### 5.1 简单语句

- 空语句：只含有一个单独的分号：`;`。

### 5.3 条件语句

- `switch`语句中，`case`关键字和它对应的值一起被称为`case`标签。`case`标签必须是整型常量表达式：

  ```c++
  char ch = getVal();
  int val = 42;
  switch(ch){
    case 3.14: // 错误：case标签不是一个整数
    case val:  // 错误：case标签不是一个常量
  }
  ```

- 如果没有任何一个`case`标签能匹配上`switch`表达式的值，程序将执行紧跟在`default`标签后面的语句。

- `switch`背部的变量定义：`switch`的执行过程有可能跨过某些`case`标签。如果某些变量在被跨过的标签里定义初始化了，在之后标签又被用到，这是非法行为。如果某一带有初值的变量位于作用域之外，在另一处该变量位于作用域之内，则从前一处跳到后一处的行为是非法的。

- C++语言规定，不允许跨过变量的初始化语句直接跳转到该变量的作用域之外。

### 5.4 迭代语句

#### 5.4.1 `while`语句

- 定义在`while`语句条件部分或循环体内的变量每次迭代都经历从创建到销毁的过程。

#### 5.4.2 传统的`for`语句

- ```c++
  for(int-statement; condition; expression){
    	statrment;
  }
  ```


- 执行流程：
  1. 循环开始，首先执行一次`int-statement`。
  2. 接下来判断`condition`。
  3. 如果条件为真，执行循环体。
  4. 执行`expression`。

#### 5.4.3 范围`for`语句

- C++11 引入的一种更简单的`for`语句：

  ```c++
  for(declaration: expression)
    statement;
  ```

  - `expression`表示的必须是一个序列、花括号括起来的初始值列表、数组、`vector`对象或`string`对象等，它们共同的特点是都拥有能返回迭代器的`begin`和`end`成员。

### 5.5 跳转语句

-  `break`语句：终止离它最近的`while`、`do while`、`for`或`switch`语句。

- `continue`语句：终止离它最近的循环中的当前迭代并立即开始下一次迭代。一般只用于`while`、`do while`、`for`语句中。当`switch`嵌套在迭代语句中时，才能在`switch`中使用`continue`。

- `goto`语句：从`goto`语句无条件跳转到同一函数内的另一条语句。一般不要在程序中使用`goto`。

  ```c++
  goto label;
  .
  .
  label: statement;
  ```

### 5.6 `try`语句块和异常处理

- C++中，异常处理包括

  1. `throw`表达式，异常检测部分使用`throw`表达式来表示它遇到了无法处理的问题，我们说`throw`引发了异常。`throw`表达式包括：关键字`throw`和紧随其后的一个表达式，其中表达式的类型就是抛出的异常类型。`throw xxxx；`
  2. `try`语句块，异常处理部分。以`try`关键字开始，并以一个或多个`catch`子句结束。
  3. 一套异常类，用于在`throw`表达式和相关的`catch`子句之间传递异常的具体信息。

  ​