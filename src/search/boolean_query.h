#pragma once
#include "query.h"
#include <vector>
#include <iostream>

namespace mmse {

enum class Occur {MUST, MUST_NOT, SHOULD};

class BooleanQuery;

struct BooleanClause {
    BooleanClause(QueryPtr q, Occur o) : query(q), occur(o) {}
    QueryPtr query;
    Occur occur;
};

class BooleanScorer : public Scorer {
};

class BooleanWeight : public Weight {
public:
    BooleanWeight(BooleanQuery* src_boolean_query);
    ScorerPtr make_scorer() override;
    void set_min_should_match(uint32_t n) { _min_should_match = n; }
private:
    std::vector<WeightPtr> _weights;
    BooleanQuery* _src_boolean_query = nullptr;
    uint32_t _min_should_match{0};
};

class BooleanQuery : public Query {
public:
    void add(QueryPtr q, Occur occur) {
        _clauses.emplace_back(BooleanClause(q, occur));
    }
    WeightPtr make_weight() override {
        auto w = mmse::make<BooleanWeight>(this);
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