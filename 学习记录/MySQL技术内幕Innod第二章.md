# Innodb概述

该引擎是第一个完整支持acid事务的MySQL存储引擎。

特点是支持行锁设计、支持mvcc、支持外键、提供一致性非锁定读。



## innodb体系架构

主要由一些后台线程和一个内存池组成。

内存池的作用：

- 维护所有进程/线程需要访问的多个内存数据结构
- 缓冲磁盘上的数据，方便快读地读取，同时在对磁盘文件的数据修改之前在这里缓冲
- 重做日志（redo log）缓冲
- 等等

后台线程的作用：

- 负责刷新内存池中的数据。保证缓冲池中的内存缓冲是最近的数据。
- 将已经修改的数据文件刷新到磁盘文件，同时保证数据库发生异常的情况下innodb能恢复到正常运行状态



### 后台线程

有多种后台线程，不同后台线程处理不同工作，数量也不同。

- master thread：负责将缓冲池中的数据异步刷新到磁盘，保证数据一致性，包括脏页的刷新，合并插入缓冲、undo页的回收等。

- io thread：使用大量aio（async io）来处理io请求。主要工作就是负责io请求的回调处理。

- IO thread种类有：write、read、insert buffer和log。在linux 平台下，最初每种io thread只有一个，即一共四个io thread。在后来的版本中write和read都增至4个，并且可以使用innodb_read_io_threads 和innodb_write_io_threads参数来控制读写线程的数量。

  ![1565582980909](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565582980909.png)



- purge thread：用来回收undolog。在事务提交后，undolog可能不在需要，就通过这个线程来回收已经使用并分配的undo页。在innodb1.1之前这个回收工作由master thread完成。在1.1开始出现purge thread，且要通过innodb_purge_threads=1来开启，及时这个值大于1也仍然只有一个该线程。在1.2之后这个线程的个数可以通过配置来增加。

- page cleaner thread：在1.2引入的。作用是将之前版本中脏页的刷新操作都放入单独的线程中完成。

### 内存

innodb是基于磁盘存储的。为了弥补磁盘与CPU之间的速度之差，使用内存来进行缓冲。

在innodb中，数据是按页从磁盘中读取的。下一次在读取数据时，首先判断该页是否在内存中，若在称为命中，则直接使用；若不在则读取磁盘。数据更新时也是先更新内存，然后按一定的频率刷新到磁盘上。刷新会通过一种checkpoint的机制刷新。

内存池的大小可以通过参数设置：innodb_buffer_pool_size;

缓冲池中缓冲的数据页类型有：

- 索引页
- 数据页
- undo页
- 插入缓冲
- 自适应哈希索引
- innodb存储的锁信息
- 数据字典信息

![1565601127315](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565601127315.png)

缓冲池的实例个数也可以设置，使用innodb_buffer_pool_instances;

#### 关于缓冲池的管理

缓冲池通常是使用lru算法来进行管理的。在innodb中对lru进行了优化。在该lru列表中设置midpoint页，对于新读取的页不直接放在列表头而是放在midpoint位置上。默认配置下该位置是lru列表长度的5/8处。midpoint的位置可以通过innodb_old_blocks_pct控制。通过show语句可以看到这个值为37，意为百分之三十七之的位置。在这个点之后的值都称为old，之前的称为new。

为什么要使用这种lru而不是用朴素版本的lru。因为新读取的数据不一定是活跃的热点数据，比如只是在做一次索引扫描或数据扫描，这样的数据仅仅在本次查询中使用，后面就不会再使用了。若这些不活跃的数据放入lru列表首部就非常有可能将真正的活跃数据从lru列表中移除。

#### free list

数据库刚启动时，lru列表中没有已经读取的页，则此时该列表为空。这时候页都存在free列表中，当需要从缓冲池中分配页时，首先从free列表中查找是否有可用的空闲页，若有则将该页从free列表中删除，放入到lru列表中；若没有空闲页，则lru列表启用lru算法淘汰lru列表尾的页。

#### flush list

在lru列表中的页被修改后成为脏页，也就是缓冲池中的页和磁盘上数据产生了不一致。这时数据库会通过chickpoint机制将脏页刷新至磁盘。flush列表中的页即为脏页。脏页既存在于lru列表中，也存在于flush 列表中。

#### 缓冲池大小

在innodb中缓冲池页的大小默认为16KB。

### 重做日志缓冲

用来缓冲重做日志。重做日志缓冲不需要太大内存，因为其刷新频率高。有三种情况会将重做日志缓冲中的数据刷新至磁盘中的重做日志文件中：

1. master thread每一秒将重做日志缓冲刷新至重做日志文件
2. 每个事物提交时会将重做日志缓冲刷新到重做日志文件
3. 当重做日志缓冲池剩余空间小于二分之一，刷新。

# checkpoint技术

当前事物数据库普遍都采用了write ahead log 策略，就是在事物提交时，先写重做日志，再修改页。这样做得好处是，即使在将新数据刷新至磁盘时发生了宕机，也可以通过重做日志来完成数据的恢复。这也是acid中d的要求。

由于缓冲内存和日志文件不能无限大，（即使可以无限大，那么数据库宕机后的重做事件也会非常长）所以采用checkpoint技术来解决：

- 缩短数据库恢复时间
- 缓冲池不够时，将脏页刷新到磁盘
- 重做日志不可用时，刷新脏页

在宕机时checkpoint 之前的脏页已经刷新到磁盘，所以数据库只需对checkpoint之后的重做日志进行恢复。这样大大缩短了恢复时间。

当缓冲不够用时，根据lru算法会溢出最近最少使用的页，若此页为脏页则强制执行checkpoint，将脏页刷新至磁盘。

重做日志是循环使用的而不是无限增大的，重做日志可以被重用的那部分是指这部分日志已经不再需要了。哪怕是数据库宕机，在恢复时也不会使用这部分日志。



# master thread工作方式

## innodb1.0.x之前的版本

内部由 多个循环组成：

- 主循环：loop
- 后台循环：backgroup loop
- 刷新循环：flush loop
- 暂停循环：suspend loop

### loop主循环

主要是每秒钟的操作和每十秒钟的操作。

每秒一次的操作：

- 日志缓冲刷新到磁盘，即使这个事务未提交（总是）
- 合并插入缓冲（可能）：当innodb盘算一秒内发生的次数小于5则认为当前io压力很小，可以执行插入缓冲
- 至多刷新100个innodb脏页（可能）：若缓冲池中脏页的比例大于innodb_max_dity_pages_pct这个参数（默认90%），则就刷新脏页至多100个
- 如果当前没有用户活动，则切换至backgroup（可能）



每10秒一次的操作：

- 刷新100个脏页到磁盘（可能）
- 合并至多5个插入缓冲（总是）
- 将日志缓冲刷新到磁盘（总是）
- 删除无用的undo页（总是）
- 刷新100个或10个脏页到磁盘（总是）

### backgroup loop

- 删除无用的undo页9（总是）
- 合并20个插入缓冲（总是）
- 跳回主循环（总是）
- 不断刷新100个页直到符合条件（可能，跳转到flush loop完成）



## innodb1.2.x之前的master thread

就是1.0到1.2之间的版本。

相关参数做了改变，之前的最大刷新脏页数和合并插入数都是有上限的，不能很好地利用新出现的磁盘的性能。

改变的参数有：

![1565618099688](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565618099688.png)

![1565618275235](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565618275235.png)

![1565618316278](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565618316278.png)

![1565618345248](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565618345248.png)



## innodb 1.2版本的master thread

![1565618425957](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565618425957.png)



# innodb 关键特性

- 插入缓冲（insert buffer）
- 两次写（double write）
- 自适应哈希索引（adaptive hash index）
- 异步io（async io）
- 刷新邻接页（flush neighbor page）

## 插入缓冲

由于innodb使用b+树作为索引的底层数据结构，这就导致了非聚集索引的离散性（主键索引就是聚集索引）。

当非聚集索引（辅助索引）在插入时很容易出现插入的多个节点在索引中不是有序的，这就要离散的访问多个非聚集索引页，这就会导致插入的性能下降。

为了解决这个问题，innodb设计了插入缓冲，对于非聚集索引的插入或更新操作，不是每一次都是直接插入到索引页中的，而是先判断插入的非聚集索引页是否在缓冲池中，若在则直接插入；若不在则先放在一个insert  buffer 对象中。对于用户来说看似已经将数据插入到叶子节点，但是实际并没有。它只是先暂存在一个地方，然后会以一定的频率和情况进行insert buffer和辅助索引叶子节点的合并操作。

要使用insert buffer要同时满足两个条件：

- 索引是辅助索引
- 索引不是唯一的



不能是唯一的原因是，唯一索引在插入时需要去查找索引页来判断要插入的索引是否是唯一 ，这就要设计到索引页的离散读，从而失去了合并的意义。

insert buffer 存在一个问题，就是在写密集的情况下，插入缓冲会占用过多的缓冲池内存。若此时数据库宕机，势必会有大量的insert buffer 未合并到实际的非聚集索引中，在这种情况下数据库恢复就会耗费大量时间。而且由于占用大量缓冲池内存，会对其他操作也带来一些影响。



### change buffer

是insert buffer的升级。从1.0.x开始，innodb可以对dml操作（insert、delete、update）都进行缓冲分别是insert buffer、delete buffer、purge buffer。

将一条记录进行update可能分为两个过程：

- 将记录标记为已删除，delete buffer对应这个过程
- 真正将记录删除， purge buffer对应这个过程。



### insert buffer实现

insert buffer是一棵B+树，负责对所有辅助索引进行insert buffer。这个B+树存放在共享表空间中。

主要是非叶子节点和叶子节点：

- 非叶子：![1565661507000](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565661507000.png)九个字节。space表示查询哪张表，占四字节；marker占1字节，用来兼容老版本的insert buffer；offset，表示页所在的偏移量，4字节。
- 叶子：![1565661636265](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565661636265.png)

当一个辅助索引要插入到页时，若这个页不在缓冲中，那么就要构造一个search key（非叶子节点中存放的就是），然后查询insert buffer这个B+树，最后将这条记录插入到叶子节点。

插入不是直接将待插记录插入，而是根据叶子节点规则进行构造。



## 两次写

数据库在宕机时一个页可能还没完全写入到磁盘，例如16KB的页只写入了4KB，即出现部分写失效。这时候即使使用重做日志也无可能法恢复，因为重做日志记录的是对页的物理操作。如果这个页本身已经损坏了，，再对其进行重做也就是没有意义的事了。这是就出现了两次写。

两次写分为两部分：一部分是在内存中的doublewrite buffer，大小为2MB，另一部分是物理磁盘上共享表空间中连续的128个页，即2个区。，大小也是2M。

当对缓冲池的脏页进行刷新时，不直接刷新到磁盘，而是通过memcp将脏页拷贝到内存中的doublewrite buffer中，之后doublewrite buffer通过两次，每次1M顺序地写入共享空间的物理磁盘上，然后马上调用fsync函数同步磁盘，避免缓冲写带来的问题。



### 自适应哈希索引

innodb存储引擎会监控对表上各索引页的查询，如果观察到建立哈希索引可以带来速度提升，则建立哈希索引。

哈希索引是通过缓冲池的B+树页来构造的。他有一个要就就是对这个页的连续访问模式必须是一样的。也就是查询的条件是一样的，若两种条件交替进行就不会构造哈希索引。



### 异步IO

与异步IO对应的是同步IO（sync IO）：就是在每次进行IO操作时，需要等待此次操作结束才能继续接下来的操作。

异步IO（asynchronous IO），用户在发出一个IO请求操作之后可以立即再发送另一个IO请求，当全部IO请求发送完毕后，等待所有IO操作的完成。

AIO的优势是可以减少等待IO的时间，还有一个优点是可以进行IO merge，当对一个页连续访问时，可以将这多个操作合并为一个操作，从而减少iO操作次数。

### 刷新邻接页

![1565683057649](C:\Users\PantaSun\AppData\Roaming\Typora\typora-user-images\1565683057649.png)

## 启动、关闭与恢复

