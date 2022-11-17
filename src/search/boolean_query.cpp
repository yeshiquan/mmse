#include "boolean_query.h"
#include "boolean_scorer2.h"
#include "check.h"

namespace unise {

BooleanWeight::BooleanWeight(BooleanQuery* src_boolean_query) {
    _src_boolean_query = src_boolean_query;
    for (auto& clause : src_boolean_query->get_clauses()) {
        Weight* weight = clause.query->create_weight();
        _weights.emplace_back(weight);
    }
}

Scorer* BooleanWeight::make_scorer() {
    auto& clauses = _src_boolean_query->get_clauses();
    std::vector<Scorer*> optional_scorers;
    std::vector<Scorer*> required_scorers;
    std::vector<Scorer*> prohibited_scorers;

    std::cout << "BooleanWeight::make_scorer() weight.size:" << _weights.size() << std::endl;

    DCHECK(clauses.size() == _weights.size());

    for (int i = 0; i < _weights.size(); ++i) {
        Weight* weight = _weights[i];
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
    return new BooleanScorer2(optional_scorers, required_scorers, prohibited_scorers);
}

} // namespace