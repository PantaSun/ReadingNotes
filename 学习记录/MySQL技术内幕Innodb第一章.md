## 定义数据库和实例

- 数据库：物理操作系统文件或其他形式文件类型的集合，是依照某种数据模型组织起来并存放于二级存储器中的数据集合
- 实例：MySQL数据库由后台线程以及一个共享内存组成。数据库实例才是真正用于操作数据库文件的。是一种程序，是位于用于与操作系统之间的一层数据管理软件。



MySQL是一个单进程多线程架构。MySQL数据库实例在系统上的表现就是一个进程。

当MySQL启动实例时，会去读取配置文件，根据配置文件的参数来启动数据库实例。若配置文件不存在，则MySQL会使用默认参数来设置实例启动。

MySQL配置文件的读取顺序（从上到下表示从先到后）：

- /etc/my.cnf
- /etc/mysql/my.cnf
- /usr/local/myql/etc/my.cnf
- ~/.my.cnf

若这些配置文件都存在且存在相同的参数，则以最后一个读取到的配置文件为基准。

## 引擎

### 什么存储引擎

存储引擎说白了就是如何存储数据、如何为存储的数据建立索引和如何更新、查询数据等技术的实现方法。因为在关系数据库中数据的存储是以表的形式存储的，所以存储引擎也可以称为表类型(即存储和操作此表的类型)。

### OLTP（在线事务处理）和OLAP（在线分析处理）主要区别有：

1、基本含义不同：OLTP是传统的关系型数据库的主要应用，主要是基本的、日常的事务处理，记录即时的增、删、改、查，比如在银行存取一笔款，就是一个事务交易。OLAP即联机分析处理，是数据仓库的核心部心，支持复杂的分析操作，侧重决策支持，并且提供直观易懂的查询结果。典型的应用就是复杂的动态报表系统。

2、实时性要求不同：OLTP实时性要求高，OLTP 数据库旨在使事务应用程序仅写入所需的数据，以便尽快处理单个事务。OLAP的实时性要求不是很高，很多应用顶多是每天更新一下数据。

3、数据量不同：OLTP数据量不是很大，一般只读/写数十条记录，处理简单的事务。OLAP数据量大，因为OLAP支持的是动态查询，所以用户也许要通过将很多数据的统计后才能得到想要知道的信息，例如时间序列分析等等，所以处理的数据量很大。

4、用户和系统的面向性不同：OLTP是面向顾客的,用于事务和查询处理。OLAP是面向市场的,用于数据分析。

5、数据库设计不同：OLTP采用实体-联系ER模型和面向应用的数据库设计。OLAP采用星型或雪花模型和面向主题的数据库设计。

