#pragma once

#include "collector.h"
#include "ref_ptr.h"

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

class Query {
public:
    virtual ~Query() {}
    virtual WeightPtr create_weight() = 0;
    virtual void rewrite() {}
};
 
} // unise