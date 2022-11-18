#include "req_excl_scorer.h"

namespace unise {

ReqExclScorer::ReqExclScorer() {
}

ReqExclScorer::ReqExclScorer(Scorer* req, Scorer* excl)
        : _required_scorer(req)
        , _exclude_scorer(excl) {
    _exclude_scorer->next_doc();
}

ReqExclScorer::~ReqExclScorer() {
}

DocId ReqExclScorer::next_doc() {
    DocId req_doc = _required_scorer->next_doc();
    return _skip_to_nonexclude();
}

DocId ReqExclScorer::skip_to(DocId target) {
    _required_scorer->skip_to(target);
    return _skip_to_nonexclude();
}

DocId ReqExclScorer::_skip_to_nonexclude() {
    DocId req_doc = _required_scorer->doc();
    DocId excl_doc = _exclude_scorer->doc();
    if (req_doc < excl_doc) {
        return _current_doc = req_doc;
    }
    // req_doc >= excl_doc
    while (true) {
        if (req_doc == NO_MORE_DOCS) {
            return _current_doc = NO_MORE_DOCS;
        }
        excl_doc = _exclude_scorer->skip_to(req_doc);
        // req_doc <= excl_doc
        if (req_doc == excl_doc) {
            req_doc = _required_scorer->next_doc();
            continue;
        } else {
            _current_doc = req_doc;
            break;
        }
    }
    return _current_doc;
}

DocId ReqExclScorer::doc() const {
    return _current_doc;
}

} // namespace