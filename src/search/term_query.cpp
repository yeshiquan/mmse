#include "term_query.h"
#include "term_scorer.h"
#include "engine.h"

namespace mmse {

ScorerPtr TermWeight::make_scorer() {
    auto* segment = Engine::instance().get_segment();
    TermId term_id = gen_term_id(_src_query->get_term().term_value);
    const PostingList* list = segment->find(term_id);
    auto term_scorer = make_object<TermScorer>(_src_query->get_term(), list);
    return term_scorer;
}

} // namespace