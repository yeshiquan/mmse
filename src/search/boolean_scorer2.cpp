#include "boolean_scorer2.h"
#include "conjunction_scorer.h"

#include <iostream>

namespace unise {

BooleanScorer2::BooleanScorer2() {
}

BooleanScorer2::~BooleanScorer2() {
}

BooleanScorer2::BooleanScorer2(std::vector<Scorer*>& optional, std::vector<Scorer*>& required, std::vector<Scorer*>& prohibited)
    : _optional_scorers(optional)
    , _required_scorers(required)
    , _prohibited_scorers(prohibited) {
    std::cout << "BooleanScorer2()\n";
    _counting_sum_scorer = _make_counting_sum_scorer();
}

Scorer* BooleanScorer2::_make_counting_sum_scorer() {
    return new ConjunctionScorer(_required_scorers);
}

DocId BooleanScorer2::next_doc() {
    return _counting_sum_scorer->next_doc();
}

DocId BooleanScorer2::skip_to(DocId target) {
    return _counting_sum_scorer->skip_to(target);
}

DocId BooleanScorer2::doc() {
    return _counting_sum_scorer->doc();
}

} // namespace