#include "collector.h"

namespace mmse {

Collector::Collector() {
}

Collector::~Collector() {
}

void Collector::collect(DocId doc_id) {
    _doc_id_list.emplace_back(doc_id);
}

} // namespace