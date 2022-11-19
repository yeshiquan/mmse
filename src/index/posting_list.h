#pragma once

#include "container/skiplist.h"
#include "field.h"

namespace mmse {

class PostingList {
public:
    PostingList(TermId term_id) : _term_id(term_id) {}
    void add_doc(DocId doc_id) {
        _skip_list.insert(doc_id);
    }
    const SkipList<DocId>& get_skip_list() const { return _skip_list; };
    static auto get_empty_iterator() {
        return SkipList<DocId>::get_empty_simple_iterator();
    }
private:
    TermId _term_id;
    float _avg_field_len;
    SkipList<DocId> _skip_list;
};

} // namespace