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
- bootloader做的事情：
  - 从实模式切换到保护模式，使能保护模式&段机制
  - 从硬盘上读取kernel in ELF格式的ucore kernel（跟在MBR后面的扇区）并放到内存中固定的位置
  - 转到ucore OS的入口点执行，这时控制权到了ucore OS
  - 使能保护模式（protection mode），bootloader/OS要设置CR0的bit 0（PE），段机制在保护模式下是自动使能的

 