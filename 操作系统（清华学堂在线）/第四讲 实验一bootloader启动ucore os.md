## 启动顺序

### x86启动顺序-第一条指令

- CS = F000H，EIP = 0000FFF0H
- 实际地址是：Base + EIP = FFFF0000H + 0000FFF0H = FFFFFFF0H，这是BIOS的EPROM（Erasable Programmable Read Only Memory）所在地
- 当CS被新值加载，则地址转换规则将开始起作用
- 通常第一条指令是一条长跳转指令（这样CS和EIP都会更新），跳转到BISO代码中执行（初始化）。
- BIOS主要做一些硬件初始化工作，保证机器能够正常的进行后续工作。

### 从BIOS到Bootloader

- BIOS加载存储设备（比如软盘、硬盘、光盘、USB盘）上的第一个扇区（主引导扇区，Master Boot Record，MBR）的512字节到内存的0x7c00
- 然后跳转到0x7c00开始执行
- **bootloader**做的事情：
  - 从实模式切换到保护模式，使能保护模式&段机制
  - 从硬盘上读取kernel in ELF格式的ucore kernel（跟在MBR后面的扇区）并放到内存中固定的位置
  - 转到ucore OS的入口点执行，这时控制权到了ucore OS
  - 使能保护模式（protection mode），bootloader/OS要设置CR0的bit 0（PE），段机制在保护模式下是自动使能的

 ### 使能保护模式

bootloader要设置CR0寄存器的bit 0（PE位）设置值为1.

段机制在保护模式下自动使能的。



## GCC内联汇编

### 什么是内联汇编（Inline assembly）

- 这是GCC对C语言的扩张
- 可直接在C语句中插入汇编指令

### 有何用处

- 调用C语言不支持的指令
- 用汇编在C语言中手动优化

### 如何工作

- 用给定的模版和约束来生成汇编指令
- 在C函数内形成汇编源码

### 举例

#### 1

- 汇编代码：movl $0xffff, %eax
- 内联汇编：asm("move $0xffff, %%eax\n");

#### 2 讲CR0寄存器第0位置位1

- 内联汇编：

  ```c
  uint32_t cr0;
  asm volatile ("movl %%cr0, %0\n":"=r"(cr)); 	// volatile表示不需要进一步优化
  												// %0 表示第一个用到的寄存器
  												// r表示任意寄存器
  cr0 |= 0x80000000;
  asm volatile ("movl %0, %%cr0\n"::"r"(cr0));
  ```

- 普通汇编

  ```asm
  movl %cro, %eax
  movl %ebx, 12(%esp)
  orl $-2147483648, 12(%esp)
  movl 12(%esp), %eax
  movl %eax, %cr0
  ```

  

### 完整格式

asm(

assembler temple

:output operands  (optional)

:input operands (optional)

:clobbers (optional)

);