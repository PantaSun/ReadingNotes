## 前言

**C++支持三种类型的成员函数**：

1. 静态成员函数（static member functions）
2. 非静态成员函数（nonstatic member functions）
3. 虚函数（virtual functions）

**static member functions不可能做到的两点**：

1. 直接存取nonstatic数据
2. 被声明为const的

## Member的各种调用方式

### 非静态成员函数（nonstatic member functions）

C++的设计准则之一就是：非静态成员函数至少必须和一般的（指非成员）非静态函数有相同的效率。

**非静态成员函数不会为类和对象带来额外的负担，因为编译器会将非静态成员函数实例转化为非静态非成员函数实例。**

一个非静态成员函数转化为普通的非静态函数的步骤如下：

1. 改写函数原型（signature）：在函数的参数列表中增加一个参数，以便于使用class object，这个指针就是this指针。
2. 将函数中每一个“对nonstatic data members”的存取操作都改为经由this指针来存取：`this->operator;`
3. 将成员函数重写成一个外部函数，将函数名称经过“mangling”处理，使它在程序中独一无二

举例：一个类的成员函数为：

```c++
float Point3d::magnitude() // Point3d是一个类，其中有三个数据成员float类型的_x,_y,_z
{
    return sqrt(_x*_x + _y*_y + _z*_z);
}
```

经过转化后变为：

```c++
float magnitude_7Point3dFv(Point3d * const this)//注意这里函数名经过mangling处理
{
    return sqrt(this->_x*this_x + this->_y*this_y + this->_z*this_z);
}
```

若原来的非静态成员函数是const的，即`Point3d::magnitude()const{...}`，则转化后的函数的this参数也要变成`const Point3d * const this`。

这时使用`obj.magnitude();`（obj是一个Point3d对象）时，就变成了`magnitude_7Point3dFv(&obj);`

还有一点要注意，若一个类的非静态成员函数的返回类型是该类类型，转化时也不要忘记NRV（named returned value）优化。

#### 名称特殊处理方法（name mangling）

一般会在函数名后加上类名以及参数类型用以转化成独一无二的函数。这只是cfront的编码方法，其它编译器就不一定了。

举例：

```c++
class Point
{
 public:
    void x(float newX);
    float x();
    ...
}
//这里是两个重载函数，为了使函数名独一无二，可以转化为
class Point
{
 public:
    void x_5PointFf(float newX);  // 将类名和参数类型都编码到函数名中了
    float x_5Pointfv();
    ...
}
```

从上面可以看出通过将函数的参数也添加到名称处理里面去，形成独一无二的函数名称，能够解决因为函数重载导致的函数重名。另外对于不正确的调用，那么在链接期间就会无法决议而失败，这也就是“确保类型安全的链接行为”，不过这种行为只能捕捉函数标记，如果返回类型声明错误，那么是无法捕捉到的。

### 虚拟成员函数（virtual member functions）

假设Point3d类有一个虚拟成员函数`virFun()`：

**那么当使用`ptr->virFun();`时**，将会被内部转化为`(*ptr->vptr[1])(ptr)`，这里：

- vptr表示由编译器产生的指针，指向virtual table。事实上该指针的名称也会被mangling处理，因为一个复杂的class派生体系中，可能不止一个vptrs。
- 1是virtual table slot的索引，关联到函数virFun
- 第二个ptr表示this指针。

如果virFun函数中又用非多态的方式调用了一个virFoo虚函数，它会被转化为`(*this->vptr[2])(this)`。因为virFun函数已经被虚拟机制判定为执行`Point3d::virFun`，且virFoo也不是用多态方式调用的，因此该函数也是执行`Point3d::virFoo`。所以可以在virFun函数中显示的调用virFoo函数会比较有效率。

显示的调用操作会压制虚拟机制。也就是使用类作用域操作符（class scope operator）显示调用一个虚函数，其resolved（解释）方式会和非静态成员函数一样。例如使用`Point3d::virFun();` ，这样就相当于直接调用一个普通的非静态函数了，就不需要通过虚指针以及虚表来判断到底执行哪一个类中的virFun函数了。

**当使用Point3d类的对象obj来直接调用virFun函数：`obj.virFun();`时**，就没必要转化成`(*obj.vptr[1])(&obj)`，虽然这样做从语意上来说是正确的。因为obj是一个Point3d类型的对象，只可能调用`Point3d::virFun()`。“经由一个class object 调用一个virtual function”，这种操作应该总是被编译器像对待非静态成员函数一个地加以决议（resolved）。

### 静态成员函数（static member functions）

静态成员函数的主要特性就是它没有this指针，以下的次要特性都是根源于主要特性：

- 首先静态成员函数不能声明为const、volatile和virtual类型的。
- 而且静态成员函数独立于class object存在，因此不能读写nonstatic data member。
- 如果取一个静态成员函数的地址的话，得到的就是其在内存中的地址，而不是一个指向“class member function”的指针。

```c++
&Point3d::obj_count();

// 得到的类型就是
unsigned int(*)()

// 而不是
unsigned int(Point3d::*)()
```



很久之前c++还没有引入静态成员函数的时候，对于这种不存取nonstatic data member的函数都是如下实现：`((Point3d*) 0)->obj_count();`通过类型转换成一个虚拟的class object，然后再调用obj_count函数。该函数只是简单传回_object_count这个静态数据成员。

现在引入了静态成员函数则是转换成直接调用：`Point3d::obj_count();`

静态成员函数因为它的特性带来了一个意想不到的好处，即静态成员函数可以成为一个callback函数。



## 虚拟成员函数

### 单一继承下的虚函数

这一节主要就是讲了单一继承下C++是通过怎样的方式来支持多态的。

在C++中，**多态**就是“**以一个public base class 的指针（或reference），寻址出一个derived class object**”的意思。

假设有如下声明：

```c++
Point * ptr;	   // Point是一个基类，其中有个虚函数z()
ptr = new Point2d; // 继承自Point
// 或者是
ptr = new Point3d; // 继承自Point2d
```



多态其实就是一个输送机制，经由它，就可以在程序的任何地方采用一组public derived类型。

首先，为了支撑虚函数机制，就必须对多态对象有某种形式的“执行期类型判断法（runtime type resolution）：

1. **这时需要解决的第一个问题就是如何判断一个对象是多态的对象？**

- 解决办法是在对象中增加一份额外信息。这时新问题就出现了，什么样的对象需要添加额外信息。总不能所有对象都增加，因为这样即增加了额外空间负担又破坏了与C语言中结体之间的兼容性；也不能仅凭struct和class关键字来判断多态。
- 这时就有了新的机制：当在没有导入ploymorphic新关键词时，要识别一个类是否支持多态的唯一适当的方法就是看看它是否有任何虚函数。只要类拥有一个虚函数，它就需要一份额外的执行期信息。

2. **第二个问题就是要保存哪些额外的信息？**就是当用ptr调用虚函数z()时：`ptr->z();`，什么信息能使程序在执行期间调用正确的虚函数z()。

**需要知道的信息：**

- 指针ptr所指的对象的真实类型。以便于选择正确的z()。
- z()实例的地址。

**在实现上：**可以为每个多态对象添加两个成员：

- 一个字符或字符串，表示类的类型
- 一个指针，指向某个表格，表格中持有程序的虚函数的执行期地址。

----

#### 表格中的虚函数地址是如何构建起来的

在c++中，虚函数可以在编译时期获知。且这一组地址是固定的，执行期不会新增或修改替换。表格的第一个slot存放各自类的type_info。然后开始存放虚函数实例地址。

#### 如何找到这些地址

- 为了找到表格，使用一个虚指针vptr指向表格vtbl。
- 为了在表格中找到虚函数地址，每个虚函数被指派一个表格索引。

----

#### 关于虚表

在单一继承下一个类只会有一个虚表。每个虚表存放所有的active virtual functions函数实例地址，这些函数包括：

- 这个类所定义的虚函数实例。改写一个可能存在的基类虚函数。
- 继承自基类的函数实例。即本派生类不改写基类虚函数时才会出现。
- 一个pure_virtual_called()函数实例，它既扮演pure virtual function的空间保卫者角色，也可以当做执行期间异常处理函数。

----

#### 关于虚表索引

虚函数在虚表中的索引值在整个同一继承体系中保持与特定虚函数的关系。比如说基类中的虚函数A，B和C的实例地址在虚表中被指派的索引分别是1,2和3。那么派生类中的A，B和C在派生类的虚表中的索引也是1,2和3，即使派生类会改写这些虚函数。另外若派生类有新定义的（不是继承来的）虚函数，那么这些新虚函数实例地址在虚表中的索引就从4开始排。

----

#### 派生发生哪些事？

1. 派生类可以继承基类所声明的虚函数的函数实例。基类的函数实例的地址会被拷贝到派生类虚表中相对的slot中。
2. 若派生类改写基类函数就不会拷贝基类的虚函数实例地址了，因为要使用自己修改后的函数实例地址。虽然修改过，但也要放在相对位置上。
3. 派生类新加入一个虚函数。这时虚表的尺寸会增加一个slot，新虚函数实例地址就会放在这个新增加的slot中。

----

#### 小结

虽然ptr每次调用虚函数z()时我们不知道ptr所指的对象的真正类型。但是我们知道可以经由ptr取到虚表，又知道所调用虚函数z()在续表中的索引值，假设为4，那么就可以通过`(*ptr->vptr[4])(ptr);`来调用正确的虚函数。

唯一一个只有在执行期才知道的东西就是虚表中索引为4的slot中到底放的是哪一个z()函数的实例。

用指针或reference调用虚函数时，编译时期只是知道该函数实例地址的存放位置，具体地址是啥只有执行的时候才知道。



### 多继承下的虚函数