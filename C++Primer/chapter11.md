## 第十一章 关联容器

- 关联容器和顺序容器有着根本的不同：关联容器中的位置是按关键字来保存和访问的。而顺序容器中的元素是按它们在容器中的位置顺序保存和访问的。
#### 关联容器类型

- 按关键字有序保存元素
    - map 关联数组：保存关键字-值对
    - set  关键字即值，即只保存关键字的容器
    - multimap 关键字可重复出现的map
    - multiset 关键字可重复出现的set
  - 无序集合
      - unordered_map 用哈希函数组织的map
      - unordered_set 用哈希函数组织的set
      - unordered_multimap 哈希组织的map：关键字可以重复出现
      - unordered_multiset 哈希组织的set：关键字可重复出现

### 使用关联容器

