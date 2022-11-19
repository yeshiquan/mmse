#pragma once
#include "field.h"
#include <vector>

namespace mmse {
 
class Document {
public:
    Document(DocId id);
    ~Document();

    Document(Document const&) = delete;             // Copy construct
    Document(Document&&) = delete;                  // Move construct
    Document& operator=(Document const&) = delete;  // Copy assign
    Document& operator=(Document &&) = delete;      // Move assign   
    
    void add_field(TextField* field);
    void add_field(StringField* field);
    void add_field(NumberField* field);
    const std::vector<TextField*> get_text_fields() { return text_fields; }
    DocId get_doc_id() { return doc_id; };
private:
    std::vector<TextField*> text_fields;
    std::vector<StringField*> string_fields;
    std::vector<NumberField*> number_fields;
    DocId doc_id;
};

using DocumentPtr = std::shared_ptr<Document>;

} // namespace