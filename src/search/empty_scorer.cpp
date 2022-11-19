#include "empty_scorer.h"

namespace mmse {

EmptyScorer::EmptyScorer() {
}

EmptyScorer::~EmptyScorer() {
}

DocId EmptyScorer::next_doc() {
    return NO_MORE_DOCS;
} 
DocId EmptyScorer::skip_to(DocId target) {
    return NO_MORE_DOCS;
}
DocId EmptyScorer::doc() const {
    return NO_MORE_DOCS;
}

} // namespace