## 第九章 顺序容器

- 一个容器就是一些特定类型对象的集合。

### 顺序容器概述

- vector：可变大小数组。支持快速随机访问。在尾部之外的位置插入或删除元素可能很慢。
- deque：双端队列。支持快速随机访问。在头尾插入删除速度很快。
- list：双向链表。只支持双向顺序访问。在list中任何位置插入删除都很快。
- forward_list：单向链表。只支持单项顺序访问。在链表任何位置插入删除操作速度很快。
- string：与vector相似的容器，但专门用于保存字符。随机访问快。在尾部插入删除速度快。
- array：固定大小数组。支持快速随机访问。不能添加或删除元素。

除了array，其他容器都提供高效灵活的内存管理。

#### 一些选择容器的基本原则

- 除非有更好的理由选择其他容器，否则应使用vector
- 如果程序有很多小的元素，且空间的额外开销很重要，则不要使用list或forward_list。
- 如果程序要求随机访问元素，应使用vector或deque。
- 如果程序要在容器的中间插入或删除元素，用list或forward_list。
- 如果程序在头尾插入删除元素，但不会在中间位置进行插入或删除，使用deque。

### 容器概览

- capacity：表示容器的容量
- size：表示容器当前存放元素的个数 

#### 迭代器

- 迭代器范围：[begin, end)迭代器范围中的元素包含first所表示的元素以及从first开始至last（但不包含last）之间的所有元素。

- 使用左闭合范围蕴含的编程假定：若begin和end相等，则范围为空；若begin与end不等，则范围至少包含一个元素，且begin指向范围中的第一个元素。可以对begin递增若干次，使得begin==end。

- 函数编写练习

  ```c++
  # 接受一对指向vector<int>的迭代器和一个int值。
  # 在两个迭代器指定的范围内查找给定的值，返回一个bool值来表示是否找到。
  #include <iostream>
  #include <vector>
  using namespace std;
  bool find_value(vector<int>:: iterator beg, vector<int>:: iterator en, int value){
      while (beg != en) {
          if (*beg++ == value) {
              return true;
          }
      }
      return false;
  }
  int main(int argc, const char * argv[]) {
      // insert code here...
      std::cout << "Hello, World!\n";
      vector<int> a = {1, 3, 4, 2, 5, 6, 7, 11 ,3, 5 ,8};
      if(find_value(a.begin(), a.end(), 12)) cout << "true" << endl;
      else cout << "false" <<endl;
      return 0;
  }
  
  ```

#### 容器定义和初始化

- 几种定义和初始化
  - C c;        默认构造函数。若C是一个array，则c中元素按默认方式初始化；否则c为空。
  - C c1(c2);  或 C c1=c2;   c1初始化为c2的拷贝。c1和c2必须相同容器类型，元素类型也要相同，对于array容器大小也要相同。
  - C c{a, b, c…}; 或 C={a, b, c….}; c初始化为初始化列表中元素的拷贝。列表中元素类型必须与c中元素类型相同。对于array，列表中元素必须小于或等于array的大小。
  - C c(begin, end); c初始化为迭代器begin和end指定范围中的元素的拷贝。范围中元素的类型必须与c中的元素类型相容（即可以转化），array不适用于这种方式。
  - C seq(n);  seq包含n个元素，这些元素进行了值初始化。
  - C seq(n, t); seq中包含n个初始化值为t的元素。

- 将一个容器初始化为另一个容器的拷贝：
  - 可以直接拷贝整个容器：要求两个容器的类型及其元素类型必须匹配
  - 或（array除外）拷贝由一个迭代器对指定的元素范围：新容器和原容器中的元素类型也可以不同，只要能将要拷贝的元素转化为要转化的容器的元素类型即可。
- 列表初始化：除了array之外的容器类型，初始化列表还隐含地指定了容器的大小：容器将包含与初始值一样多的元素。



#### 赋值和swap



- 赋值运算符：左边和右边的运算对象要具有相同的类型。它将右边运算对象中的所有元素拷贝到左边运算对象中。

- assign（仅顺序容器）：顺序容器（array除外）还定义了一个名为assign的成员，可以从一个不同但相容的类型赋值，或者从容器的一个子序列赋值。

- 由于旧元素会被替换，因此传递给assign的迭代器不能指向调用assign的容器。

- assign的第二个版本是：接受一个整数和一个元素值。它用指定数目且具有相同给定值的元素替换容器中原油的元素：

  ```c++
  list<string> s1(1); //一个元素，为空string
  s1.assign(10, "Nihao!"); //10个元素，每个都是"Nihao!"
  // 等价于
  // s1.clear();
  // s1.insert(s1.begin(), 10, "Nihao!");
  ```

- swap操作交换两个相同类型容器的内容。

- 除了array外，swap不对任何元素进行拷贝、删除或插入操作，因此可以保证在常数时间内完成。

- 对string调用swap会导致迭代器、引用和指针失效。

#### 容器大小操作

- 每个容器类型都有三个与大小相关的操作。
  - 成员函数size：返回容器中元素的数目
  - empty：当size为0时返回true，否则返回false
  - max_size：返回一个大于或等于该类型容器所能容纳的最大元素数的值。

#### 关系运算符

- 关系运算符左右两边的运算对象必须是相同类型的容器。
- 比较规则：
  - 两个容器具有相同的大小且所有元素两两对应相等，则这两个容器相等。
  - 两个容器大小不等，但较小的容器中每个元素都等于较大容器中对应的元素，则较小容器小于较大容器。
  - 如果两个容器都不是另一个容器的前缀子序列，则它们的比较结果取决于第一个不相等的元素的比较结果。
- 只有当容器中元素类型也定义了相应的比较运算符时，才可以使用关系运算符比较两个容器。



### 顺序容器操作

#### 添加元素

- 向容器添加元素的操作：
  - forward_list有自己专有版本的insert和emplace
  - forwa_list不支持push_back和emplace_back
  - vector 和string 不支持push_front 和 emplace_front
  - c.push_back(t); c.emplace_back(args); 在c的尾部创建一个值为t或由args创建的元素，返回void
  - c.push_front(t); c.emplace_front(args); 在c的头部创建一个值为t或由args创建的元素，返回void
  - c.insert(p,t); c.emplace(p,args); 在迭代器p指向的元素之前创建一个值为t或由args创建的元素。返回指向新添加的元素的迭代器。
  - c.insert(p, n, t); 在迭代器p指向的元素之前插入n个值为t的元素。返回指向新添加的第一个元素的迭代器；若n为0，则返回p。
  - c.insert(p, b, e); 在迭代器p指向的元素之前插入迭代器b和e指定的范围内的元素。返回指向新添加的第一个元素的迭代器；若范围为空，则返回p。
  - c.insert(p, il); il是一个花括号包围的元素值列表。将这些给定值插入到迭代器p指向的元素之前。返回指向新添加的第一个元素的迭代器。若列表为空，则返回p。
- 当用对象来初始化容器时，或将一个对象插入到容器中时，实际上放入到容器中的是对象值的一个拷贝，而不是对象本身。容器中的元素与提供值的对象之间没有任何关联。随后对容器中元素的任何改变都不会影响到原始对象。
- emplace操作：新标准引入的三个新成员
  - emplace不是将对象拷贝到容器中。
  - 当调用一个emplace成员函数时，则会将参数传递给元素类型的构造函数。
  - emplace成员会使用这些参数在容器管理的内存空间中直接构造元素。
  - 传递给emplace函数的参数必须与元素类型的构造函数相匹配。

#### 访问元素

- 包含array在内的每个顺序容器都有一个front成员函数，而除了forward_list之外的所有顺序容器都有一个back成员函数。这两个操作分别返回首元素和尾元素的引用。

- 获取容器c中首元素和尾元素的例子

  ```c++
  if(!c.empty()){ //在解引用一个迭代器或调用front或back前要检查是否有元素
      // va1 和 va2 是c中第一个元素值的拷贝
      auto va1 = *c.begin();
      auto va2 = c.front();
      // va3 和 va4 是c中最后一个元素值的拷贝
      auto last = c.end();
      auto va3 = *(--last); // 不能递减forward_list迭代器
      auto va4 = c.back();  // forward_list不支持
  }
  ```

- at和下标只适用于string、vector、deque和array

  ```c++
  c.at[n]; //返回下标为n的元素的引用，n是一个无符号数，下标越界，则抛出out_of_range异常
  c[n];    //返回下标为n的元素的引用，n是一个无符号数，若n>c.size()，则函数未定义
  ```

- 访问成员函数返回的都是引用，若容器不是const的，则返回值是普通引用，可以用来修改元素值。

#### 删除元素

- 删除操作
  - forward_list 有特殊版本的erase
  - forward_list 不支持pop_back；vector和string不支持pop_front。
  - c.pop_back(); 删除c中尾元素。若c为空，则函数行为未定义。返回void。 
  - c.pop_front(); 删除c中首元素。若c为空，则函数行为未定义。返回void。 
  - c.erase(p); 删除跌打器p所指的元素，返回一个指向被删除元素之后的迭代器，若p指向为元素，则返回尾后跌打器。若p是尾后迭代器，则函数行为未定义。
  - c.erase(b, e); 删除迭代器b和e所指定范围内的元素。返回一个指向最后一个被删除元素之后元素的迭代器，若e本身是尾后迭代器，则函数函数也返回尾后迭代器。
  - c.clear(); 删除c中的所有元素，返回void。

#### 特殊的forward_list

- 因为forward_list 是单向链表，每个节点除了保存数据外只保存了指向其后继的指针。当我门要在把对象插入到某个元素前时，我们需要知道该元素前驱的链接，这显然是不可能的。综上所述，在forward_list中插入或删除是通过改变给定元素之后的元素来完成的。于是就有了：

  - insert_after
  - emplace_after
  - erase_after
  - before_begin：返回指向链表首元素之前不存在的元素的迭代器。此迭代器不能解引用。

- 练习：查找并删除forward_list<int>中的奇数元素。

  ```c++
  #include <iostream>
  #include <forward_list>
  using namespace std;
  int main(){
  	forward_list<int> fl = {1,2,3,4,7,4,5,5,6,7,8,9};
      cout << "befor delete: "<< endl;
      for (auto f:fl) {
          cout << f << " ";
      }
      cout << endl;
      auto prev = fl.before_begin();
      auto curr = fl.begin();
      while (curr != fl.end()) {
          if ((*curr % 2) == 1) {
              curr = fl.erase_after(prev);
          }else{
              prev = curr++;
          }
      }
      cout << "after delete:" << endl;
      for (auto ff:fl) {
          cout << ff << " ";
      }
      cout << endl;
      
      return 0;
  }
  /*
  	输出结果：
  	befor delete: 
  	1 2 3 4 7 4 5 5 6 7 8 9 
  	after delete:
  	2 4 4 6 8 
  */
  ```

#### 改变容器大小

- 除了array，其他容器可以用resize来增大或缩小容器。
- 如果resize缩小容器，则指向被删除元素的迭代器、引用和指针都会失效。
- 对vector、string或deque进行resize可能导致迭代器、指针和引用失效。

###vector对象是如何增长的

- 当不得不获取新的内存空间时，vector和string的实现通常会分配比新的空间需求更大的内存空间。容器预留这些空间作为备用，可以保存更多的新元素。
- 这样就不需要每次添加新元素都重新分配容器内存空间了。
- 容器大小管理操作
  - c.shrink_to_fit(); 只适用于vector、string和deque。请求将capacity减少为与size相同大小。也就是将多余的内存空间退还，但是调用该函数只是一个请求，标准库不保证退还内存。
  - c.capacity(); 只适用于vector和string 不重新分配内存的话，c可以保存多少元素
  - c.reserve(n); 分配至少能容纳n个元素的内存空间。

### 额外的string操作

- 当从一个const char* 创建string 数组时，指针指向的数组必须以空字符结尾，拷贝操作遇到空字符时停止。
- s.substr(pos, n); 返回一个string，包含s中从pos开始的n个字符的拷贝。pos的默认值为0。n的默认值为s.size()-pos，即拷贝从pos开始的所有字符。

#### 改变string的其他方法

- append：在string末尾进行插入的一种简写形式。

  ```c++
  string s("C++ Primer"), s2 = s;
  s.insert(s.size(), " 4th Ed.");
  s2.append(" 4th Ed."); //这里s2的值与s等价
  ```

- replace：是调用erase和insert的一种简写形式。

  ```c++
  // 接上文
  s.erase(11, 3);   //这时 s=="C++ Primer Ed."
  s.insert(11, "5th"); // 这时 s=="C++ Primer 5th Ed."
  s2.replace(11, 3, "5th"); //s2==s， 这里的参数3是指从11开始删除3个字符 
  ```

#### string搜索

- string搜索函数返回string::size_type值，该类型是一个unsigned类型。如果搜索失败，则返回string::npos的static成员。
- 标准库将npos定义为一个 const string::size_type类型，并初始化为值-1。因为npos你一个unsigned类型，这就意味值npos等于任何string最大的可能大小。
- 搜索是大小写敏感的。
- 

#### compare函数

#### 数值转换

- to_string(i); 将整数i转换为字符表示形式
- 将字符串s转换为数值：
  - stoi
  - stol
  - stoul
  - stoll
  - stoull
  - stof
  - stod
  - stold



### 容器适配器

- 除了顺序容器外，标准库还定义了三个顺序容器适配器：
  - stack：定义在stack头文件中
  - queue
  - priority_queue：和queue都定义在queue头文件中。
- 适配器是标准库中的一个通用概念。
- 容器、迭代器和函数都有适配器。

#### 定义一个适配器

- 每个适配器都定义两个构造函数：默认构造函数创建一个空对象，另一个构造函数接受一个容器的构造函数拷贝该容器来初始化适配器。
- 可以在创建一个适配器时将一个命名的顺序容器作为第二个类型参数，来重载默认容器类型。
- 适配器不能构造在array之上，因为所有适配器都要求容器具有添加和删除元素的能力。
- 也不能用forward_list来构造适配器，因为所有适配器都要求容器具有添加、删除以及访问尾元素的能力。
- stack可以使用array和forward_list之外的任何容器类型来构造。
- queue适配器可以构造在list和deque之上，但不能基于vector构造。
- priority_queuek可以基于vector或deque，但不能基于list

#### 栈适配器

- 栈默认基于deque实现，也可以基于list或vector实现

  ```c++
  stack<string, vectot<string>> str_stk; // 在vector上实现的空栈
  ```

#### 队列适配器

- queue默认基于deque实现
- priority_queue默认基于vector实现