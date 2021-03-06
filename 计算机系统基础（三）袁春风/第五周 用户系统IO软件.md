## I/O操作的实现

### 操作系统在程序执行过程中的作用

一个基本的Hello World程序在执行过程中，该程序不会直接访问显示器、磁盘和主存等硬件资源，而是依靠OS提供的服务来间接访问。

操作系统就是在应用程序和硬件之间插入的一个中间软件层。

#### 操作系统的两个作用

- 资源管理，已达到：
  - 统筹安排和调度资源，以防止资源被用户程序滥用
  - 对于广泛使用的复杂低级设备，为用户提供一个简单一致的使用接口
- 为用户使用系统提供一个操作接口

### I/O子系统概述

从用户IO软件切换到内核IO软件的唯一办法就是“异常机制”：系统调用（自陷）

各类用户的IO请求需要通过某种方式传给OS：

- 最终用户：键盘、鼠标通过操作界面传给OS
- 用户程序：通过函数转换为系统调用传递给OS

IO软件被组织成从高到低4个层次，层次越低，就越接近设备而越原理用户：

1. 用户IO软件
2. 与设备无关的操作系统IO软件
3. 设备驱动程序
4. IO中断处理程序

大部分IO软件都属于操作系统内核程序，最初的IO请求在用户程序中提出。



### 用户IO软件与系统调用





### 文件的基本概念

- 所有IO操作通过读写文件实现，所有外设，包括网络、中断设备，都被看成文件。
- 所有物理设备抽象成逻辑上统一的文件，使得用户程序访问物理设备与访问真正的磁盘文件完全一致。
- Unix系统中，文件就是一个字节序列。stream！字节流
- 通常将键盘和显示器构成的设备称为终端，对应标准输入和标准输出文件；磁盘、光盘则是普通文件。
- 根据文件可读性分为ASCII文件和二进制文件
- ASCII文件也成为文本文件，可由多个正文行组成，每行以换行符'\n'结束，每个字符占一个字节。标准输入和标准输出文件时ASCII文件。



## 头文件stdio.h内容理解

在头文件中定义了一些常数，如NULL、EOF等。

在C标准IO库函数中对文件的操作使用了一个缓冲区，将打开文件抽象为一个类型为FILE的流，它在stdio.h中定义。

```c
typedef struct _iobuf{
    int cnt; 		//未读写字节数
    char * ptr;		//下一可读位置
    char * base;	//起始位置 
    int flag;   	//存取模式
    int fd;			//文件描述符
}
```

在系统调用中read和write等函数是直接对文件进行操作的，而C标准IO库函数中的fread和fwrite是将数据写入缓冲区或从缓冲区读入数据。

标准输入输出文件不需要显示的打开，如stdin、stdout和stderr

### 带缓冲IO的实现

读数据时，从文件fp中读入1024（缓冲区大小）个字节数据到缓存，然后再按需从缓存中读取一个或n个字节并返回。

写数据时，先按需不断地向缓存写入一个或n个字节，当遇到换行符时或缓存被写满使，则将缓存内容一次写入文件fp中。

#### stdout和stderr的区别

stdout和stderr的区别，这里stdout是带缓存标准输出文件而stderr不带缓存的标准输出文件。

```
#include<stdio.h>
int main(){
    fprintf(stdout, "hello");
    fprintd(stderr, "world!");
    return 0;
}
```

在Linux下上述程序输出为world!hello，因为stderr没有使用缓冲区，所以数据是直接输出到文件里的，这里的文件指的是显示器（在Unix/Linux下一些皆文件）。而stdout是先将hello送入缓存，若是在hello后有一个回车，那么就是hello先打印出来了。

使用缓冲，可以减少直接读写文件的次数，从而减少系统调用的次数（系统调用的开销很大）。



