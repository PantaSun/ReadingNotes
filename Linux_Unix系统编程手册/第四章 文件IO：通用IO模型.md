### lseek函数

```c
#include<unistd.h>
// 成功则返回新的文件偏移量
// 失败就返回-1
off_t lseek(int fd, off_t offset, int whence);
```

该函数是用来调整文件偏移量。

**参数：**

- fd：文件描述符，某个已经代开的文件
- offset：以字节为单位的数值

- whence：表明应参照哪个基点来解释offset参数。whence有三种参照方式：
  - SEEK_SET：将文件偏移量设置为从文件头开始的offset个字节
  - SEEK_CUR：相当于当前文件偏移量
  - SEEK_END：将文件偏移量设置为起始于文件末尾的offset个字节，offset参数应该从文件末尾的最后一个字节之后的下一个字节算起。

> lseek函数不适用于所有类型的文件。不允许用于管道、FIFO、socket或终端。

 

### 总结

对普通文件的IO操作：先使用open函数来获得一个文件描述符。然后可以对这个文件描述符使用read和write函数进行读写操作，最后使用close函数释放文件描述符以及相关资源。

对于已经打开的文件，内核维护了一个文件偏移量，这个偏移量决定了下一次读写的起始位置。读和写都会隐式修改文件偏移量。函数lseek可以显示修改文件偏移量。

在文件末尾之后的某一位置写入数据将产生文件空洞。从文件空洞处读取文件将返回全0字节。文件空洞不占用磁盘空间。