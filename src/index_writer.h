#pragma once

#include "index/document.h"
#include <vector>

namespace unise {
 
class IndexWriter {
public:
    IndexWriter();
    ~IndexWriter();

    IndexWriter(IndexWriter const&) = delete;             // Copy construct
    IndexWriter(IndexWriter&&) = delete;                  // Move construct
    IndexWriter& operator=(IndexWriter const&) = delete;  // Copy assign
    IndexWriter& operator=(IndexWriter &&) = delete;      // Move assign    

    void build();
    void add_document(DocumentPtr doc);
private:
    void _process_doc(Document* doc, std::vector<std::unique_ptr<Token>>& token_list);
private:
    std::vector<DocumentPtr> _documents;
};

} // namespace