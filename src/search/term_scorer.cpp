#include "term_scorer.h"

namespace mmse {

TermScorer::TermScorer(const PostingList* list) {
    _init(list);
}

TermScorer::TermScorer(const Term& term, const PostingList* list)
            : _term(term) {
    _init(list);
}

void TermScorer::_init(const PostingList* list) {
    if (list) {
        auto& skiplist = list->get_skip_list();
        _doc_iter = skiplist.get_simple_iterator();
    } else {
        _doc_iter = PostingList::get_empty_iterator();
    }
}

TermScorer::~TermScorer() {
    std::cout << "~TermScorer()" << std::endl;
}

DocId TermScorer::next_doc() {
    _doc_iter.next_node();
    return doc();
}

DocId TermScorer::skip_to(DocId target) {
    _doc_iter.skip_to(target);
    return doc();
}

DocId TermScorer::doc() const {
    auto* node = _doc_iter.node();
    return (node ? node->data() : NO_MORE_DOCS);
}

} // namespace