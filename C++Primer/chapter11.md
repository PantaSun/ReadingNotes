## 第十一章 关联容器

- 关联容器和顺序容器有着根本的不同：关联容器中的位置是按关键字来保存和访问的。而顺序容器中的元素是按它们在容器中的位置顺序保存和访问的。
#### 关联容器类型

- 按关键字有序保存元素
    - map 关联数组：保存关键字-值对
    - set  关键字即值，即只保存关键字的容器
    - multimap 关键字可重复出现的map
    - multiset 关键字可重复出现的set
  - 无序集合
      - unordered_map 用哈希函数组织的map
      - unordered_set 用哈希函数组织的set
      - unordered_multimap 哈希组织的map：关键字可以重复出现
      - unordered_multiset 哈希组织的set：关键字可重复出现

### 使用关联容器

- map使用举例

  ```c++
  map<string, size_t> word_count;
  string word;
  while(cin >> word){
      ++word_count[word];
  }
  for(const auto &w:word_count)
  	cout << w.first << "occurs " << w.second << (w.second > 1? "times." : "time.") << endl;
  ```

  当从map中取出一个元素时，会得到一个pair类型的对象，pair是一个模版类型，保存两个名为first和second的（公有）数据成员。map所使用的pair用first成员保存关键字，用second成员保存对应的值。

- 使用set

  ```c++
  set<string> exclude = {"The", "But", "And", "Or"};
  ```

### 关联容器概述

- 关联容器支持普通容器操作，但不支持顺序容器中的位置相关的操作。
- 关联容器的迭代器都是双向的。

#### 定义关联容器

- 定义map必须指定关键字和值类型；定义set时，只需要指明关键字类型，因为set中没有值。

- 每个关联容器都定义了一个默认构造函数，它创建一个指定类型的空容器。

- 也可以用关联容器初始化为另一个同类型容器的拷贝。

- 或从一个值范围来初始化关联容器

- 新标准下，也可以对关联容器进行值初始化：

  ```c++
  map<string, size_t> word_count; // 空容器
  set<string> exculde = {"the", "but"};//
  map<string, string> authors = {{"joyce", "James"}, {"Austen", "Jane"}};
  ```

#### pair类型

- pair定义在头文件utilty中。

- 一个pair保存两个数据成员，pair的默认构造函数对数据成员进行值初始化。

  ```c++
  pari<string, string> anon; //保存两个string
  pait<string, size_t> word_count; //一个string和一个size_t
  pair<string, vector<int>> line; // 保存string和vector<int>
  ```

- 也可以为每个成员提供初始化器：`pair<string, string> author{"James", "Joyce"};`

- pair的数据成员时public的。两个成言分别命名为first和second，可以用普通的成员访问符访问它们。

### 关联容器操作

- 关联容器额外的类型别名
  - key_type 此容器类型的关键字类型
  - mapped_type 每个关键字关联的类型：只适用于map
  - value_type 对于set，于key_type相同；对于map，为pair\<const key_type, mapped_type \>

#### 关联容器迭代器

- 当解引用一个关联容器迭代器时，我们会得到一个类型为容器的value_type的值的引用。对map来说就是一个pair，其first保存cosnt的关键字，second保存值。
- 一个map的value_type是一个pair，可以改变这个pair的值，但不能改变关键字成员的值。
- set的迭代器是const的。

##### 遍历迭代器

- 当一个迭代器遍历一个map、multimap、set或multiset时，迭代器按关键字升序遍历元素。

#### 添加元素

- 关联容器的insert成员向容器中添加一个元素或一个元素范围。由于map个set包含不重复的关键字，因此插入一个已存在的元素对容器没有任何影响。

##### 向map添加元素

- 对一个map进行insert操作时，必须记住元素类型是pair。

- 四种方式

  ```c++
  map<string, size_t> wc;
  wc.insert({word, 1});
  wc.insert(make_pair(word, 1));
  wc.insert(pair<string, size_t>(word, 1));
  wc.insert(map<string, size_t>::value_type(word, 1));
  ```

##### 检测insert的返回值

- insert或emplace返回的值依赖于容器类型和参数。

  - 对于不包含重复关键字的容器（map和set），添加单一元素的insert和emplace版本返回一个pair，这个pair包含一个迭代器（指向具有指定关键字的元素），以及指示是否插入成功的bool值。

    ```c++
    c.insert(v);
    c.emplace(args);
    ```

  - 添加多个元素的insert和emplace函数返回void

    ```c++
    c.insert(b, e); // b和e是迭代器
    c.insert(li); // li是一个花括号列表
    ```

  - 对于insert和emplace向一个指定的迭代器p指示的从哪里开始搜索新元素应该存储的位置的插入，返回一个迭代器，指向具有给定关键字的元素。

    ```c++
    c.insert(p, v);
    c.emplace(p, args);
    ```

##### 向multimap和multiset添加元素

#### 删除元素

- 关联容器定义了三个erase。其中有两个与顺序容器一样，可以通过传递给erase一个迭代器或一个迭代器对来删除一个或一个范围内的元素。且这两个函数返回void。
- 关联容器的第三个erase接受一个key_type参数。删除所有匹配给定关键字的元素（如果存在多个的话）。返回实际删除的元素的数量。若返回0则表明想要删除的元素并不在容器中。

#### map的下标操作

- map和unordered_map容器提供了下标运算符和一个对应的at函数。
- set类型不支持下标。
- 不能对multimap和unordered_multimap使用下标操作，因为这些容器可能多个值与一个关键字相关联。
- 当对map进行下标操作时，会返回一个mapped_type对象。
- 与vector和string不同，map的下标运算符返回的类型与解引用map迭代器得到的类型不同。

#### 访问元素

- 对于不允许重复关键字的关联容器，使用find和count没什么区别。
- 对于允许重复关键字的容器，count会统计有多少个元素有相同的关键字。如果不需要统计，最好用find
- lower_bound 和 upper_bound不适用于无序容器。
- 下标at操作值适用于非const的map和unordered_map。
- 相关操作
  - c.lower_bound(k)：返回一个迭代器，指向第一个关键字不小于k的元素。返回的迭代器将指向第一个具有给定关键字的元素。
  - c.upper_bound(k)：返回一个迭代器，指向第一个关键字大于k的元素。迭代器指向最后一个匹配给定关键字的元素之后的位置。
  - 若元素不在multimap中，则lower_bound和upper_bound会返回相等的迭代器。
  - c.euqal_range(k)：返回一个迭代器pair，表示关键字等于k的元素的范围。若k不存在，pair的两个成员均等于c.end()
- 若lower_bound和upper_bound返回相同的迭代器，则给定关键字不在容器中。

####无序容器

##### 管理桶

- 无序容器在存储组织上为一组桶，每个桶保存零个或多个元素。

- 无序容器使用一个哈希函数将元素映射到桶。

- 当访问一个元素，容器首先计算元素哈希值，它指出应该搜索哪个桶。

- 对于相同的元素，哈希函数必须总是产生相同的结果。

  

