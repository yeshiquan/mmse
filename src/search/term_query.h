#pragma once
#include "query.h"
#include "index/field.h"

namespace mmse {

class TermQuery;

class TermWeight : public Weight {
public:
    TermWeight(TermQuery* src) : _src_query(src) {}
    virtual ScorerPtr make_scorer() override;
    TermQuery* term_query() { return _src_query; }
private:
    TermQuery* _src_query = nullptr;
};

class TermQuery : public Query {
public:
    TermQuery(Term&& term) : _term(std::move(term)) {};
    WeightPtr create_weight() override {
        return make_object<TermWeight>(this);
    }
    Term& get_term() { return _term; }
private:
    Term _term;
};
 
} // unise