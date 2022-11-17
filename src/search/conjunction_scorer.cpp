#include "conjunction_scorer.h"

#include <iostream>

namespace unise {

ConjunctionScorer::ConjunctionScorer() {
}

ConjunctionScorer::~ConjunctionScorer() {
}

ConjunctionScorer::ConjunctionScorer(std::vector<Scorer*>& scorers) 
                : _scorers(scorers) {
    for (auto scorer : _scorers) {
        if (scorer->next_doc() == NO_MORE_DOCS) {
            _last_doc = NO_MORE_DOCS;
            return;
        }
    }

    std::sort(_scorers.begin(), _scorers.end(), [](Scorer* a, Scorer* b) -> bool { 
        return a->doc() < b->doc();
    });

    if (_align_header() == NO_MORE_DOCS) {
        _last_doc = NO_MORE_DOCS;
        return;
    }
}

DocId ConjunctionScorer::next_doc() {
    if (_last_doc == NO_MORE_DOCS) {
        return _last_doc;
    } else if (_last_doc == -1) {
        // 首次调用next_doc()，因为构造函数已经调用align_header对齐头部元素
        // 所以每条拉链的头部元素都相同，返回任意1条拉链的头部元素就行
        return _last_doc = _scorers[0]->doc();
    }
    _scorers.back()->next_doc();
    return _last_doc = _align_header();
}

// 蛙跳算法求交集的关键步骤：跳过不相同的元素，使得每条拉链的首个元素都相等
// 这样每条拉链的首个元素都是对齐的，这里的首个元素就是交集结果的元素
// 例如3条拉链，对齐之前如下：
// list1 -> 1, 2, 3, 5
// list2 -> 1, 3, 9
// list3 -> 2, 3, 4, 10
// list1跳过1和2, list2跳过1，list3跳过2，对齐后:
// list1 -> 3, 5
// list2 -> 3, 9
// list3 -> 3, 4, 10
DocId ConjunctionScorer::_align_header() {
    DocId doc = _scorers.back()->doc();
    int idx = 0;
    Scorer* next_scorer = _scorers[idx];
    while (next_scorer->doc() < doc) {
        doc = next_scorer->skip_to(doc);
        idx = (idx + 1) % _scorers.size();
        next_scorer = _scorers[idx];
    }
    return doc;
}

DocId ConjunctionScorer::skip_to(DocId target) {
    if (_last_doc == NO_MORE_DOCS) {
        return _last_doc;
    } else if (_scorers.back()->doc() < target) {
        _scorers.back()->skip_to(target);
    }
    return _last_doc = _align_header();
}

DocId ConjunctionScorer::doc() {
    return _last_doc;
}

} // namespace
