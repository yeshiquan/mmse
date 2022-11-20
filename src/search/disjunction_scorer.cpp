#include "disjunction_scorer.h"

#include <iostream>

namespace mmse {

DisjunctionScorer::DisjunctionScorer() {
}

DisjunctionScorer::DisjunctionScorer(std::vector<ScorerPtr>& scorers, uint32_t min_match) 
                : _scorers(scorers)
                , _minimum_matchers(min_match) {
    for (auto scorer : _scorers) {
        scorer->inc_ref();
        scorer->next_doc();
        _queue.push(scorer);
    }
}

DisjunctionScorer::~DisjunctionScorer() {
    std::cout << "~DisjunctionScorer()" << std::endl;
    for (auto scorer : _scorers) {
        scorer->dec_ref();
    }
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

std::vector<std::string> DisjunctionScorer::explain() const {
    std::vector<std::string> outputs;
    outputs.emplace_back("DisjunctionScorer(min_match=" + std::to_string(_minimum_matchers) + ") -> ");
    for (auto scorer : _scorers) {
        for (auto& line : scorer->explain()) {
            outputs.emplace_back("  " + line);
        }
    }
    return outputs;
}

} // namespace