#pragma once
#include "query.h"
#include "index/field.h"

namespace mmse {

class TermQuery;

class TermWeight : public Weight {
public:
    TermWeight(TermQuery* src);
    ~TermWeight();
    virtual ScorerPtr make_scorer() override;
    TermQuery* term_query() { return _src_query; }
private:
    TermQuery* _src_query = nullptr;
};

class TermQuery : public Query {
public:
    TermQuery(Term&& term) : _term(std::move(term)) {};
    ~TermQuery() {
        std::cout << "~TermQuery() " << std::endl;
    }
    WeightPtr make_weight() override {
        //return mmse::make<TermWeight>(this);
        return new TermWeight(this);
    }
    Term& get_term() { return _term; }
private:
    Term _term;
};
 
} // unise