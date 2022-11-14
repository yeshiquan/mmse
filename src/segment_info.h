#pragma once

#include "posting_list.h"
#include "field.h"

#include <unordered_map>
#include <memory.h>

namespace unise {
 
class SegmentInfo {
public:
    SegmentInfo();
    ~SegmentInfo();

    SegmentInfo(SegmentInfo const&) = delete;             // Copy construct
    SegmentInfo(SegmentInfo&&) = delete;                  // Move construct
    SegmentInfo& operator=(SegmentInfo const&) = delete;  // Copy assign
    SegmentInfo& operator=(SegmentInfo &&) = delete;      // Move assign    

    void add_token(Token* token) {
        if (_inverted_index.count(token->term_id) == 0) {
            _inverted_index.emplace(token->term_id, std::make_unique<PostingList>(token->term_id));
        }
        // std::cout << "add term -> term_id:" << token->term_id 
        //                       << " doc_id:" << token->docid << std::endl;
        std::unique_ptr<PostingList>& list = _inverted_index[token->term_id];
        list->add_doc(token->docid);
    }

    const PostingList* find(TermId term_id) {
        auto iter = _inverted_index.find(term_id);
        if (iter == _inverted_index.end()) {
            return nullptr;
        }
        std::unique_ptr<PostingList>& list = iter->second;
        return list.get();
    }
private:
    // 核心数据结构，倒排索引
    std::unordered_map<TermId, std::unique_ptr<PostingList>> _inverted_index;
};

} // namespace