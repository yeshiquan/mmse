#pragma once

#include "collector.h"
#include "ref_ptr.h"

// Query, Weight, Scorer是3棵树
// 这3种对象是一次查询请求处理过程中构建的
// 所有对象都用RefPtr包装，不是线程安全的
// 确保这些对象不要用在多线程环境下。

namespace mmse {

class Query;
class Scorer;
using ScorerPtr = RefPtr<Scorer>;
using QueryPtr = RefPtr<Query>;

class Weight {
public:
    virtual ~Weight() {}
    virtual Query* get_query() { return nullptr; }
    virtual ScorerPtr make_scorer() = 0;
};
using WeightPtr = RefPtr<Weight>;

// Scorer同时承担2大功能
// 1) Scorer本身是一个迭代器，通过next_doc()可以遍历所有文档
// 2) Scorer也承担了给文档打分的功能
class Scorer {
public:
    virtual ~Scorer() {}
    virtual void next() {}

    void score(Collector* collector) {
        DocId doc_id = next_doc();
        while (doc_id != NO_MORE_DOCS) {
            collector->collect(doc_id);
            doc_id = next_doc();
        }
    }
    virtual std::vector<std::string> explain() const = 0;
    virtual DocId skip_to(DocId target) = 0;
    virtual DocId doc() const = 0;
    virtual DocId next_doc() = 0;
};

// 用户通过构建Query查询树来检索文档
class Query {
public:
    virtual ~Query() {}
    virtual WeightPtr create_weight() = 0;
    virtual void rewrite() {}
};
 
} // unise