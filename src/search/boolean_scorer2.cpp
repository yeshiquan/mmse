#include "boolean_scorer2.h"
#include "conjunction_scorer.h"
#include "disjunction_scorer.h"
#include "req_excl_scorer.h"
#include "req_opt_sum_scorer.h"
#include "empty_scorer.h"
#include "check.h"

#include <iostream>

namespace mmse {

BooleanScorer2::BooleanScorer2() {
}

BooleanScorer2::~BooleanScorer2() {
}

BooleanScorer2::BooleanScorer2(std::vector<Scorer*>& optional, std::vector<Scorer*>& required, std::vector<Scorer*>& prohibited)
    : _optional_scorers(optional)
    , _required_scorers(required)
    , _prohibited_scorers(prohibited) {
}

void BooleanScorer2::build() {
    _counting_sum_scorer = _make_counting_sum_scorer();
}

// should产生的效果和min_should_match关系密切。
// 1) should的条件数和min_should_match一样，说明每个条件都要满足，这些should的条件就和must的效果一样了可以生成ConjunctionScorer
// 2) should的条件数大于min_should_match，说明只用满足其中一部分条件，可以生成DisjunctionScorer
// 3) should的条件数小于min_should_match，这是不可能做到的，在_make_counting_sum_scorer的入口处直接就处理了，返回EmptyScorer
//    所以_make_should函数不需要考虑这种情况。
Scorer* BooleanScorer2::_make_should(std::vector<Scorer*>& scorers, uint32_t min_match) {
    DCHECK(scorers.size() >= min_match);
    if (scorers.size() == 1) {
        return scorers[0];
    } else {    
        if (scorers.size() > min_match) {
            return new DisjunctionScorer(scorers, min_match);
        } else {
            return new ConjunctionScorer(scorers);
        } 
    }
    NOTREACHABLE();
    return nullptr;
}

Scorer* BooleanScorer2::_make_must(std::vector<Scorer*>& scorers) {
    DCHECK(scorers.size() > 0);
    if (scorers.size() == 1) {
        return scorers[0];
    }
    return new ConjunctionScorer(scorers);
}

Scorer* BooleanScorer2::_make_must_plus_should(std::vector<Scorer*>& must_scorers, std::vector<Scorer*>& should_scorers) {
    if (should_scorers.size() == _min_should_match) {
        // 因为每个should的条件都要满足，should全部当作must来处理
        must_scorers.insert(must_scorers.begin(), should_scorers.begin(), should_scorers.end());
    } else if (should_scorers.size() > _min_should_match) {
        if (_min_should_match > 0) {
            auto* scorer1 = _make_should(should_scorers, _min_should_match);
            must_scorers.emplace_back(scorer1);
        } else {
            // _min_should_match == 0
            // 对于这种情况，召回只需要must就行，不需要should，should只用于打分，
            // 如果文档中包含should的部分，则增加打分, ReqOptSumScorer是专门处理这种情况的
            auto* scorer1 = _make_must(must_scorers);
            auto* scorer2 = _make_should(should_scorers, 1);
            return new ReqOptSumScorer(scorer1, scorer2);
        }
    } else {
        NOTREACHABLE();
    }
    return _make_must(must_scorers);
}

// A - B1 - B2 - B3 转化为 A - (B1 U B2 U B3)
Scorer* BooleanScorer2::_make_must_plus_mustnot(std::vector<Scorer*>& must_scorers, std::vector<Scorer*>& not_scorers) {
    auto* must_scorer = _make_must(must_scorers);
    auto* must_not_scorer = _make_should(not_scorers, 1);
    return new ReqExclScorer(must_scorer, must_not_scorer);
}

Scorer* BooleanScorer2::_make_counting_sum_scorer() {
    uint8_t required_cnt = _required_scorers.size() > 0 ? 0x04 : 0;
    uint8_t optional_cnt = _optional_scorers.size() > 0 ? 0x02 : 0;
    uint8_t prohibited_cnt = _prohibited_scorers.size() > 0 ? 0x01 : 0;
    uint8_t bit_case = required_cnt | optional_cnt | prohibited_cnt;

    if (_optional_scorers.size() > 0 && _optional_scorers.size() < _min_should_match) {
        // 无法满足最少匹配的数量
        return new EmptyScorer();
    }
    if (bit_case == 1) {
        // only must_not
        // 否定查询必须和其他条件搭配，否则不允许查询
        return new EmptyScorer();
    }
   if (bit_case == 2) {
        // only should
        return _make_should(_optional_scorers, _min_should_match);
    }
    if (bit_case == 4) {
        // only must
        return _make_must(_required_scorers);
    }
    if (bit_case == 3) {
        // should + must_not
        // 这种情况should会被认为是must
        auto* must_scorer = _make_should(_optional_scorers, _min_should_match);
        auto* exclude_scorer = _make_should(_prohibited_scorers, 1);
        return new ReqExclScorer(must_scorer, exclude_scorer);
    }
    if (bit_case == 5) {
        // must + must_not
        return _make_must_plus_mustnot(_required_scorers, _prohibited_scorers);
    }
    if (bit_case == 6) {
        // must + should
        return _make_must_plus_should(_required_scorers, _optional_scorers);
    }
    if (bit_case == 7) {
        // must + must_not + should
        // must先和must_not组合成ReqExclScorer。
        // 然后和should联合生成ReqOptSumScorer
        auto* scorer1 = _make_must_plus_mustnot(_required_scorers, _prohibited_scorers);
        std::vector<Scorer*> must_scorers{scorer1};
        return _make_must_plus_should(must_scorers, _optional_scorers);
    }
    NOTREACHABLE();
    return nullptr;
}

DocId BooleanScorer2::next_doc() {
    return _counting_sum_scorer->next_doc();
}

DocId BooleanScorer2::skip_to(DocId target) {
    return _counting_sum_scorer->skip_to(target);
}

DocId BooleanScorer2::doc() const {
    return _counting_sum_scorer->doc();
}

std::vector<std::string> BooleanScorer2::explain() const {
    std::vector<std::string> outputs;
    outputs.emplace_back("BooleanScorer2(min_should=" + std::to_string(_min_should_match) + ") -> ");
    for (auto& line : _counting_sum_scorer->explain()) {
        outputs.emplace_back("  " + line);
    }
    return outputs;
}


} // namespace