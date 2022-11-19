#pragma once
#include "query.h"
#include <vector>
#include <iostream>

namespace unise {

enum class Occur {MUST, MUST_NOT, SHOULD};

class BooleanQuery;

struct BooleanClause {
    BooleanClause(Query* q, Occur o) : query(q), occur(o) {}
    Query* query;
    Occur occur;
};

class BooleanScorer : public Scorer {
};

class BooleanWeight : public Weight {
public:
    BooleanWeight(BooleanQuery* src_boolean_query);
    Scorer* make_scorer() override;
    void set_min_should_match(uint32_t n) { _min_should_match = n; }
private:
    std::vector<Weight*> _weights;
    BooleanQuery* _src_boolean_query = nullptr;
    uint32_t _min_should_match{0};
};

class BooleanQuery : public Query {
public:
    void add(Query* q, Occur occur) {
        _clauses.emplace_back(BooleanClause(q, occur));
    }
    Weight* create_weight() override {
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