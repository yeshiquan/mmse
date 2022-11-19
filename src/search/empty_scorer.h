#pragma once

#include "query.h"

namespace mmse {
 
class EmptyScorer : public Scorer {
public:
    EmptyScorer();
    ~EmptyScorer();

    EmptyScorer(EmptyScorer const&) = delete;             // Copy construct
    EmptyScorer(EmptyScorer&&) = delete;                  // Move construct
    EmptyScorer& operator=(EmptyScorer const&) = delete;  // Copy assign
    EmptyScorer& operator=(EmptyScorer &&) = delete;      // Move assign    

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;
    DocId doc() const override;
    std::vector<std::string> explain() const override { return {"EmptyScorer"}; }
private:
};

} // namespace