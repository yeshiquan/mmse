#include "boolean_query.h"
#include "boolean_scorer2.h"
#include "check.h"

namespace mmse {

BooleanWeight::BooleanWeight(BooleanQuery* src_boolean_query) {
    _src_boolean_query = src_boolean_query;
    _src_boolean_query->inc_ref();
    for (auto& clause : src_boolean_query->get_clauses()) {
        WeightPtr weight = clause.query->make_weight();
        weight->inc_ref();
        _weights.emplace_back(weight);
    }
}

BooleanWeight::~BooleanWeight() {
    std::cout << "~BooleanWeight() weights_size:" << _weights.size() << std::endl;
    _src_boolean_query->dec_ref();
    for (auto& weight : _weights) {
        weight->dec_ref();
    }
}

ScorerPtr BooleanWeight::make_scorer() {
    auto& clauses = _src_boolean_query->get_clauses();
    std::vector<ScorerPtr> optional_scorers;
    std::vector<ScorerPtr> required_scorers;
    std::vector<ScorerPtr> prohibited_scorers;

    DCHECK(clauses.size() == _weights.size());

    for (int i = 0; i < _weights.size(); ++i) {
        WeightPtr& weight = _weights[i];
        BooleanClause& clause = clauses[i];
        if (clause.occur == Occur::MUST) {
            required_scorers.emplace_back(weight->make_scorer());
        } else if (clause.occur == Occur::MUST_NOT) {
            prohibited_scorers.emplace_back(weight->make_scorer());
        } else if (clause.occur == Occur::SHOULD) {
            optional_scorers.emplace_back(weight->make_scorer());
        } else {
            NOTREACHABLE();
        }
    }

    BooleanScorer2* s =  new BooleanScorer2(optional_scorers, required_scorers, prohibited_scorers);
    s->set_min_should_match(_min_should_match);
    s->build();
    return s;
}

} // namespace