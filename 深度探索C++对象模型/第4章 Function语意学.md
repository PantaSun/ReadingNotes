## 前言

static member functions不可能做到的两点：

1. 直接存取nonstatic数据
2. 被声明为const的

## Member的各种调用方式

### 非静态成员函数（nonstatic member functions）

C++的设计准则之一就是：非静态成员函数至少必须和一般的非静态函数有相同的效率。

一个非静态成员函数在使用时会被转化为普通的静态函数，其转化步骤如下：

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

这时使用`obj.magnitude();`（obj是一个Point3d对象）时，就变成了`magnitude_7Point3dFv(&obj);`\

还有一点要注意，若一个非静态成员函数的返回类型是该类类型，转化时也不要忘记NRV（named returned value）优化。

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
    void x_5PointFf(float newX);
    float x_5Pointfv();
    ...
}
```



### 虚拟成员函数（virtual member functions）

假设有一个虚拟成员函数`virFun()`，那么当使用`ptr->virFun();`时，将会被内部转化为`(*ptr->vptr[1])(ptr)`，这里：

- vptr表示由编译器产生的指针，指向virtual table。事实上该指针的名称也会被mangling处理，因为一个复杂的class派生体系中，可能不止一个vptrs。
- 1是virtual table slot的索引，关联到函数virFun
- 第二个ptr表示this指针。