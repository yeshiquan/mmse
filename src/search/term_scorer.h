#pragma once

#include "query.h"
#include "index/posting_list.h"

namespace mmse {

// TermScorer是最底层的，直接和倒排拉链打交道
 
class TermScorer : public Scorer {
public:
    TermScorer() = default;
    TermScorer(const PostingList*);
    TermScorer(const Term& term, const PostingList*);
    ~TermScorer();

    TermScorer(TermScorer const&) = delete;             // Copy construct
    TermScorer(TermScorer&&) = delete;                  // Move construct
    TermScorer& operator=(TermScorer const&) = delete;  // Copy assign
    TermScorer& operator=(TermScorer &&) = delete;      // Move assign   

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;
    DocId doc() const override;
    std::vector<std::string> explain() const override { return {"TermScorer(" + _term.field_name + "=" + _term.term_value + ")"}; }
private:
    void _init(const PostingList*);
private:
    SkipList<DocId>::simple_iterator _doc_iter;
    Term _term;
};

} // namespace