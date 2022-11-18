#pragma once
#include "query.h"
#include "index/field.h"

namespace unise {

class TermQuery;

class TermWeight : public Weight {
public:
    TermWeight(TermQuery* src) : _src_query(src) {}
    virtual Scorer* make_scorer() override;
    TermQuery* term_query() { return _src_query; }
private:
    TermQuery* _src_query = nullptr;
};

class TermQuery : public Query {
public:
    TermQuery(Term&& term) : _term(std::move(term)) {};
    Weight* create_weight() override {
        return new TermWeight(this);
    }
    Term& get_term() { return _term; }
private:
    Term _term;
};
 
} // unise