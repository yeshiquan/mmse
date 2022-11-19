#pragma once

#include "query.h"
#include "container/heap_queue.h"
#include <vector>
#include <queue>

namespace mmse {

// 有序并集

struct ScorerLess {
    bool operator() (const Scorer* a, const Scorer* b) {
        return a->doc() < b->doc();
    }
};        
 
class DisjunctionScorer : public Scorer {
public:
    DisjunctionScorer();
    DisjunctionScorer(std::vector<Scorer*>&, uint32_t);
    ~DisjunctionScorer();

    DisjunctionScorer(DisjunctionScorer const&) = delete;             // Copy construct
    DisjunctionScorer(DisjunctionScorer&&) = delete;                  // Move construct
    DisjunctionScorer& operator=(DisjunctionScorer const&) = delete;  // Copy assign
    DisjunctionScorer& operator=(DisjunctionScorer &&) = delete;      // Move assign   

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;
    DocId doc() const override;
    std::vector<std::string> explain() const override;
private:
    std::vector<Scorer*> _scorers;
    uint32_t _minimum_matchers{0}; // 至少匹配多少个term
    HeapQueue<Scorer*, ScorerLess> _queue; 
    DocId _current_doc = -1;
};

} // namespace