#pragma once

#include "collector.h"
#include "ref_ptr.h"

// Query, Weight, Scorer是3棵树
// 这些对象是一次查询请求处理过程中构建的
// 所有对象都使用侵入式引用计数的方式管理生命周期，不是线程安全的
// 确保这些对象不要用在多线程环境下。

namespace mmse {

class Query;
class Scorer;
using ScorerPtr = Scorer*;
using QueryPtr = Query*;

class Weight {
public:
    virtual ~Weight() {}
    virtual Query* get_query() { return nullptr; }
    virtual ScorerPtr make_scorer() = 0;

    Weight() = default;
    Weight(Weight const&) = delete;             // Copy construct
    Weight(Weight&&) = delete;                  // Move construct
    Weight& operator=(Weight const&) = delete;  // Copy assign
    Weight& operator=(Weight &&) = delete;      // Move assign     
public:
    void inc_ref() { 
        _ref_cnt++; 
        //std::cout << "Weight inc_ref after:" << _ref_cnt << std::endl;
    }
    void dec_ref() {
        //std::cout << "Weight dec_ref current:" << _ref_cnt << " after:" << _ref_cnt - 1 << " " << this << std::endl;
        if (--_ref_cnt == 0) {
            delete this;
        }
    }
    int ref_cnt() const { return _ref_cnt; }
private:
    uint32_t _ref_cnt{0};
};
using WeightPtr = Weight*;

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

    void inc_ref() { 
        _ref_cnt++; 
        //std::cout << "Scorer inc_ref after:" << _ref_cnt << std::endl;
    }
    void dec_ref() {
        //std::cout << "Scorer dec_ref current:" << _ref_cnt << " after:" << _ref_cnt - 1 << " " << this << std::endl;
        if (--_ref_cnt == 0) {
            delete this;
        }
    }
    int ref_cnt() const { return _ref_cnt; }
private:
    int _ref_cnt{0};    
};

// 用户通过构建Query查询树来检索文档
class Query {
public:
    virtual ~Query() {}
    virtual WeightPtr make_weight() = 0;
    virtual void rewrite() {}

    Query() = default;
    Query(Query const&) = delete;             // Copy construct
    Query(Query&&) = delete;                  // Move construct
    Query& operator=(Query const&) = delete;  // Copy assign
    Query& operator=(Query &&) = delete;      // Move assign     

    void inc_ref() { 
        _ref_cnt++; 
        //std::cout << "Query inc_ref after:" << _ref_cnt << std::endl;
    }
    void dec_ref() {
        //std::cout << "Query dec_ref current:" << _ref_cnt << " after:" << _ref_cnt - 1 << " " << this << std::endl;
        if (--_ref_cnt == 0) {
            delete this;
        }
    }
    int ref_cnt() const { return _ref_cnt; }
private:
    uint32_t _ref_cnt{0};
};
 
} // unise