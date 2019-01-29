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

