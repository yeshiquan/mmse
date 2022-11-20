#include "term_query.h"
#include "term_scorer.h"
#include "engine.h"

namespace mmse {

TermWeight::TermWeight(TermQuery* src) : _src_query(src) {
    _src_query->inc_ref();
}

TermWeight::~TermWeight() {
    std::cout << "~TermWeight()" << std::endl;
    _src_query->dec_ref();
}

ScorerPtr TermWeight::make_scorer() {
    auto* segment = Engine::instance().get_segment();
    TermId term_id = gen_term_id(_src_query->get_term().term_value);
    const PostingList* list = segment->find(term_id);
    auto* term_scorer = new TermScorer(_src_query->get_term(), list);
    return term_scorer;
}

} // namespace