#include "term_query.h"
#include "term_scorer.h"
#include "engine.h"

namespace unise {

Scorer* TermWeight::make_scorer() {
    std::cout << "TermWeight::make_scorer()" << std::endl;
    auto* segment = Engine::instance().get_segment();
    TermId term_id = gen_term_id(_src_query->get_term().term_value);
    const PostingList* list = segment->find(term_id);
    auto* term_scorer = new TermScorer(list);
    return term_scorer;
}

} // namespace