#pragma once

#include <vector>
#include "index/field.h"

namespace mmse {
 
class Collector {
public:
    Collector();
    ~Collector();

    Collector(Collector const&) = delete;             // Copy construct
    Collector(Collector&&) = delete;                  // Move construct
    Collector& operator=(Collector const&) = delete;  // Copy assign
    Collector& operator=(Collector &&) = delete;      // Move assign    

    void collect(DocId);
private:
    std::vector<DocId> _doc_id_list;
};

} // namespace