#pragma once

#include "query.h"
#include <vector>

namespace unise {

class BooleanScorer2 : public Scorer {
public:
    BooleanScorer2();
    ~BooleanScorer2();
    BooleanScorer2(std::vector<Scorer*>& optional, std::vector<Scorer*>& required, std::vector<Scorer*>& prohibited);

    BooleanScorer2(BooleanScorer2 const&) = delete;             // Copy construct
    BooleanScorer2(BooleanScorer2&&) = delete;                  // Move construct
    BooleanScorer2& operator=(BooleanScorer2 const&) = delete;  // Copy assign
    BooleanScorer2& operator=(BooleanScorer2 &&) = delete;      // Move assign        

    DocId next_doc() override; 
    DocId skip_to(DocId target) override;   
    DocId doc() override; 
private:
    Scorer* _make_counting_sum_scorer();
private:
    Scorer* _counting_sum_scorer = nullptr; // 领头的scorer
    std::vector<Scorer*> _optional_scorers;
    std::vector<Scorer*> _required_scorers;
    std::vector<Scorer*> _prohibited_scorers;
};

} // namespace