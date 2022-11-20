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
    std::cout << "~BooleanScorer2()" << std::endl;
    for (auto& scorer : _optional_scorers) {
        scorer->dec_ref();
    }
    for (auto& scorer : _required_scorers) {
        scorer->dec_ref();
    }
    for (auto& scorer : _prohibited_scorers) {
        scorer->dec_ref();
    }
    _counting_sum_scorer->dec_ref();
}

BooleanScorer2::BooleanScorer2(std::vector<ScorerPtr>& optional, std::vector<ScorerPtr>& required, std::vector<ScorerPtr>& prohibited)
    : _optional_scorers(optional)
    , _required_scorers(required)
    , _prohibited_scorers(prohibited) {
    for (auto& scorer : _optional_scorers) {
        scorer->inc_ref();
    }
    for (auto& scorer : _required_scorers) {
        scorer->inc_ref();
    }
    for (auto& scorer : _prohibited_scorers) {
        scorer->inc_ref();
    }        
}

void BooleanScorer2::build() {
    _counting_sum_scorer = _make_counting_sum_scorer();
    _counting_sum_scorer->inc_ref();
}

// should产生的效果和min_should_match关系密切。
// 1) should的条件数和min_should_match一样，说明每个条件都要满足，这些should的条件就和must的效果一样了可以生成ConjunctionScorer
// 2) should的条件数大于min_should_match，说明只用满足其中一部分条件，可以生成DisjunctionScorer
// 3) should的条件数小于min_should_match，这是不可能做到的，在_make_counting_sum_scorer的入口处直接就处理了，返回EmptyScorer
//    所以_make_should函数不需要考虑这种情况。
ScorerPtr BooleanScorer2::_make_should(std::vector<ScorerPtr>& scorers, uint32_t min_match) {
    DCHECK(scorers.size() >= min_match);
    if (scorers.size() == 1) {
        return scorers[0];
    } else {    
        if (scorers.size() > min_match) {
            // return mmse::make<DisjunctionScorer>(scorers, min_match);
            return new DisjunctionScorer(scorers, min_match);
        } else {
            // return mmse::make<ConjunctionScorer>(scorers);
            return new ConjunctionScorer(scorers);
        } 
    }
    NOTREACHABLE();
    return nullptr;
}

ScorerPtr BooleanScorer2::_make_must(std::vector<ScorerPtr>& scorers) {
    DCHECK(scorers.size() > 0);
    if (scorers.size() == 1) {
        return scorers[0];
    }
    //return mmse::make<ConjunctionScorer>(scorers);
    return new ConjunctionScorer(scorers);
}

ScorerPtr BooleanScorer2::_make_must_plus_should(std::vector<ScorerPtr>& must_scorers, std::vector<ScorerPtr>& should_scorers) {
    auto should_cnt = should_scorers.size();
    if (should_cnt == _min_should_match) {
        // 因为每个should的条件都要满足，should全部当作must来处理
        must_scorers.insert(must_scorers.begin(), should_scorers.begin(), should_scorers.end());
        should_scorers.clear(); // 容易出错的地方：指针拷贝后，要么增加引用计数，要么把原来的指针作废(相当于move)
    } else if (should_cnt > _min_should_match) {
        if (_min_should_match > 0) {
            auto scorer1 = _make_should(should_scorers, _min_should_match);
            must_scorers.emplace_back(scorer1);
            scorer1->inc_ref(); // 容易出错的地方：指针拷贝后要增加引用计数
        } else {
            // _min_should_match == 0
            // 对于这种情况，召回只需要must就行，不需要should，should只用于打分，
            // 如果文档中包含should的部分，则增加打分, ReqOptSumScorer是专门处理这种情况的
            auto scorer1 = _make_must(must_scorers);
            auto scorer2 = _make_should(should_scorers, 1);
            //return mmse::make<ReqOptSumScorer>(scorer1, scorer2);
            return new ReqOptSumScorer(scorer1, scorer2);
        }
    } else {
        NOTREACHABLE();
    }
    return _make_must(must_scorers);
}

// A - B1 - B2 - B3 转化为 A - (B1 U B2 U B3)
ScorerPtr BooleanScorer2::_make_must_plus_mustnot(std::vector<ScorerPtr>& must_scorers, std::vector<ScorerPtr>& not_scorers) {
    auto must_scorer = _make_must(must_scorers);
    auto must_not_scorer = _make_should(not_scorers, 1);
    //return mmse::make<ReqExclScorer>(must_scorer, must_not_scorer);
    return new ReqExclScorer(must_scorer, must_not_scorer);
}

ScorerPtr BooleanScorer2::_make_counting_sum_scorer() {
    uint8_t required_cnt = _required_scorers.size() > 0 ? 0x04 : 0;
    uint8_t optional_cnt = _optional_scorers.size() > 0 ? 0x02 : 0;
    uint8_t prohibited_cnt = _prohibited_scorers.size() > 0 ? 0x01 : 0;
    uint8_t bit_case = required_cnt | optional_cnt | prohibited_cnt;

    if (_optional_scorers.size() > 0 && _optional_scorers.size() < _min_should_match) {
        // 无法满足最少匹配的数量
        // return mmse::make<EmptyScorer>();
        return new EmptyScorer();
    }
    if (bit_case == 1) {
        // only must_not
        // 否定查询必须和其他条件搭配，否则不允许查询
        // return mmse::make<EmptyScorer>();
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
        auto must_scorer = _make_should(_optional_scorers, _min_should_match);
        auto exclude_scorer = _make_should(_prohibited_scorers, 1);
        //return mmse::make<ReqExclScorer>(must_scorer, exclude_scorer);
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
        auto scorer1 = _make_must_plus_mustnot(_required_scorers, _prohibited_scorers);
        std::vector<ScorerPtr> must_scorers{scorer1};
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