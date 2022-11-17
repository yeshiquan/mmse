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
private:
    std::vector<Weight*> _weights;
    BooleanQuery* _src_boolean_query = nullptr;
};

class BooleanQuery : public Query {
public:
    void add(Query* q, Occur occur) {
        std::cout << "add trace 1\n";
        _clauses.emplace_back(BooleanClause(q, occur));
        std::cout << "add trace 2\n";
    }
    Weight* create_weight() override {
        return new BooleanWeight(this);
    }
    std::vector<BooleanClause>& get_clauses() {
        return _clauses;
    }
private:
    std::vector<BooleanClause> _clauses;
};
 
} // unise