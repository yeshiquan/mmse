#include "term_scorer.h"

namespace unise {

TermScorer::TermScorer(const PostingList* list) {
    std::cout << "TermScorer() list:" << list << std::endl;
    if (list) {
        auto& skiplist = list->get_skip_list();
        _doc_iter = skiplist.get_simple_iterator();
    } else {
        _doc_iter = PostingList::get_empty_iterator();
    }
}

TermScorer::~TermScorer() {
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