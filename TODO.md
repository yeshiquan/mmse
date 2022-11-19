## 大的目标：实现一个全内存的检索引擎lib库，接口和Lucene保持一致

个人认为Unise的设计有一点非常不好，把检索引擎和业务使用都在一起实现了，使得代码的可读性和扩展性比较差。
我认为应该像Lucene一样，提供一个纯粹的检索引擎lib库，业务方可以基于这个lib做上层的封装。
比如索引的格式长什么样，怎么定义索引数据的格式，怎么定义查询语法，这些都交给上层去做。
lib库只提供基础的函数级别的api，不提供服务的包装。

### 学习Lucene的API设计
### 实现建库的功能
    * 跑通简单切词构建skiplist索引的功能，done
    * 引入第三方切词库

### 实现检索的功能
    * 设计查询语法树的数据结构 done
    * 实现交集、并集、否定查询
        * 实现ItemScorer和ConjunctionScorer，可以求多条拉链的交集，参考test_query.cpp done
        * 实现DisjunctionScorer, 求多条拉链的并集 done
        * 实现ReqExclScorer，实现2条拉链的差集(A-B) done
        * 修改BooleanWeight::make_scorer的业务逻辑，使得能处理所有查询条件的组合， done
          包括单个must、must_not、should
          包括must+must_not, must+should, must_not+should等等
        * 实现Scorer树的explain功能，可视化查看生成的scorer树 done
        * 交集、并集、差集的性能优化, 比如交集先用最短的拉链
    * 目前为了方便，无脑new各种对象，没有考虑对象的生命周期和内存回收。
    * 实现相关性打分, 包括各种查询条件组合下的打分，BM25等。
    * 实现文档的排序
    * 实现范围查询，考虑kd-tree
    * 实现拉链截断、限制耗时等操作
    * 重构TermScorer，使得它不依赖SkipList，只依赖迭代器，从而和SkipList解耦
    * 实现正排索引的构建和查询，正排可以用于自定义打分

### 实现并行建库、实时增删改查

### 实现多线程安全的检索机制