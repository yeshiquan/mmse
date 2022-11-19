#pragma once

#include "collector.h"

namespace mmse {

class Query;
class Scorer;

class Weight {
public:
    virtual ~Weight() {}
    virtual Query* get_query() { return nullptr; }
    virtual Scorer* make_scorer() = 0;
};

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
    virtual Weight* create_weight() = 0;
    virtual void rewrite() {}
};
 
} // unise