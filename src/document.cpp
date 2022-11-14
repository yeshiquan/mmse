#include "document.h"
#include <iostream>
namespace unise {

Document::Document(DocId id) : doc_id(id) {
}

Document::~Document() {
    std::cout << "Destroy Document" << std::endl;
    for (auto field : text_fields) {
        delete field;
    }
    text_fields.clear();
    for (auto field : string_fields) {
        delete field;
    }
    string_fields.clear();
    for (auto field : number_fields) {
        delete field;
    }
    number_fields.clear();
}

void Document::add_field(TextField* field) {
    text_fields.emplace_back(field);
}
void Document::add_field(StringField* field) {
    string_fields.emplace_back(field);
}
void Document::add_field(NumberField* field) {
    number_fields.emplace_back(field);
}

} // namespace