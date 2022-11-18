#include "disjunction_scorer.h"

#include <iostream>

namespace unise {

DisjunctionScorer::DisjunctionScorer() {
}

DisjunctionScorer::DisjunctionScorer(std::vector<Scorer*>& scorers, uint32_t min_match) 
                : _scorers(scorers)
                , _minimum_matchers(min_match) {
    for (auto scorer : _scorers) {
        scorer->next_doc();
        _queue.push(scorer);
    }
}

DisjunctionScorer::~DisjunctionScorer() {
}

DocId DisjunctionScorer::next_doc() {
    while (true) {
        if (_queue.size() < _minimum_matchers) {
            _current_doc = NO_MORE_DOCS;
            break;
        }
        DocId current_doc = _queue.top()->doc();
        uint32_t match_cnt = 0;
        while (!_queue.empty() && _queue.top()->doc() == current_doc) {
            match_cnt++;
            if(_queue.top()->next_doc() != NO_MORE_DOCS) {
                _queue.adjust_down_from_top();
            } else {
                _queue.pop();
            }
        }
        if (match_cnt >= _minimum_matchers) {
            _current_doc = current_doc;
            break;
        }
    }
    return _current_doc;
} 

DocId DisjunctionScorer::skip_to(DocId target) {
    for (auto scorer : _queue) {
        if(scorer->skip_to(target) == NO_MORE_DOCS) {
            _queue.pop();
        }
    }
    _queue.adjust_all();
    return _current_doc = next_doc();
}

DocId DisjunctionScorer::doc() const {
    return _current_doc;
}

} // namespace