#pragma once

#include "query.h"
#include "posting_list.h"

namespace unise {

// TermScorer是最底层的，直接和倒排拉链打交道
 
class TermScorer : public Scorer {
public:
    TermScorer() = default;
    TermScorer(const PostingList*);
    ~TermScorer();

    TermScorer(TermScorer const&) = delete;             // Copy construct
    TermScorer(TermScorer&&) = delete;                  // Move construct
    TermScorer& operator=(TermScorer const&) = delete;  // Copy assign
    TermScorer& operator=(TermScorer &&) = delete;      // Move assign   

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;
    DocId doc() override;
private:
    SkipList<DocId>::simple_iterator _doc_iter;
};

} // namespace