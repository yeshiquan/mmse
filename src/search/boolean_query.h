#pragma once
#include "query.h"
#include <vector>
#include <iostream>

namespace mmse {

enum class Occur {MUST, MUST_NOT, SHOULD};

class BooleanQuery;

struct BooleanClause {
    BooleanClause(QueryPtr q, Occur o) : query(q), occur(o) {
        query->inc_ref();
    }
    // 容易出错的地方：如果成员变量是带引用计数的指针,必须定义拷贝构造函数，或者禁用掉构造函数
    // 否则默认按照值拷贝对象后，指针的引用计数并没有增加，导致不一致
    BooleanClause(const BooleanClause& o) : query(o.query), occur(o.occur) {
        query->inc_ref();
    }
    ~BooleanClause() {
        query->dec_ref();
    }
    QueryPtr query{nullptr};
    Occur occur;
};

class BooleanScorer : public Scorer {
};

class BooleanWeight : public Weight {
public:
    BooleanWeight(BooleanQuery* src_boolean_query);
    ~BooleanWeight();
    ScorerPtr make_scorer() override;
    void set_min_should_match(uint32_t n) { _min_should_match = n; }
private:
    std::vector<WeightPtr> _weights;
    BooleanQuery* _src_boolean_query = nullptr;
    uint32_t _min_should_match{0};
};


class BooleanQuery : public Query {
public:
    ~BooleanQuery() {
        std::cout << "~BooleanQuery() " << this << std::endl;
    }
    void add(QueryPtr q, Occur occur) {
        _clauses.emplace_back(BooleanClause(q, occur));
    }
    WeightPtr make_weight() override {
        //auto w = mmse::make<BooleanWeight>(this);
        auto* w = new BooleanWeight(this);
        w->set_min_should_match(_min_should_match);
        return w;
    }
    std::vector<BooleanClause>& get_clauses() {
        return _clauses;
    }
    void set_min_should_match(uint32_t n) { _min_should_match = n; }
private:
    std::vector<BooleanClause> _clauses;
    uint32_t _min_should_match{0};
};
 
} // unise