#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "util.h"
#include "index/field.h"

#include "index_writer.h"
#include "engine.h"
#include "index/segment_info.h"

namespace mmse {

IndexWriter::IndexWriter() {
}

IndexWriter::~IndexWriter() {
}

void IndexWriter::build() {
    std::vector<std::unique_ptr<Token>> token_list;
    for (auto& doc : _documents) {
        _process_doc(doc.get(), token_list);
    }
    std::cout << "build document_size:" << _documents.size() 
                      << " token_size:" << token_list.size() << std::endl;

    SegmentInfo* segment = Engine::instance().get_segment();
    for (auto& token : token_list) {
        segment->add_token(token.get());
    }
}

void IndexWriter::_process_doc(Document* doc, std::vector<std::unique_ptr<Token>>& token_list) {
    //std::cout << "_process_doc text_fields_size:" << doc->get_text_fields().size() << std::endl;
    for (auto& field : doc->get_text_fields()) {
        std::vector<std::string> term_str_list;
        split_string(field->value, term_str_list, ' ');

        // std::cout << "field value -> [" << field->value << "]" 
        //             << " term_str_list.size:" << term_str_list.size()
        //             << std::endl;

        uint32_t pos = 0;
        for (auto& term_str : term_str_list) {
            auto token = std::make_unique<Token>();
            token->term_id = gen_term_id(term_str);
            token->field_id = field->field_id;
            token->docid = doc->get_doc_id();
            token->pos = pos++;
            token->value = term_str;
            token_list.emplace_back(std::move(token));
        }
    }
}

void IndexWriter::add_document(DocumentPtr doc) {
    _documents.emplace_back(doc);
}


} // namespace