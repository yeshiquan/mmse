#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <random>

#define  DCHECK_IS_ON
#define POOL_STATS true

#define private public
#define protected public
#include "search/query.h"
#include "search/boolean_query.h"
#include "search/term_query.h"
#include "search/term_scorer.h"
#include "search/conjunction_scorer.h"
#undef private
#undef protected
#include "defer.h"

#include "index/document.h"
#include "index/segment_info.h"
#include "engine.h"

using namespace unise;


class ConjunctionTest : public ::testing::Test {
private:
    virtual void SetUp() {
        //p_obj.reset(new ExprBuilder);
    }
    virtual void TearDown() {
        //if (p_obj != nullptr) p_obj.reset();
    }
protected:
    //std::shared_ptr<ExprBuilder>  p_obj;
};

TEST_F(ConjunctionTest, test_basic) {
    std::cout << "ConjunctionTest\n";
    PostingList list1(111111);
    PostingList list2(222222);
    PostingList list3(333333);
    auto build_list = [](PostingList& list, std::vector<DocId> docs) {
        for (auto doc : docs) {
            list.add_doc(doc);
        }
    };
    build_list(list1, {2,13,17,20,98});  
    build_list(list2, {1,13,22,35,98,99});  
    build_list(list3, {1,3,13,20,35,80,98});  

    TermScorer t1(&list1);
    TermScorer t2(&list2);
    TermScorer t3(&list3);

    std::vector<Scorer*> requred_scorers = {&t1, &t2, &t3};
    ConjunctionScorer conjunction_scorer(requred_scorers);

    std::vector<DocId> result;
    std::vector<DocId> expect_result{13, 98};
    DocId doc = conjunction_scorer.next_doc();
    while (doc != NO_MORE_DOCS) {
        std::cout << ">>>> conjunction doc -> " << doc << std::endl;
        result.emplace_back(doc);
        doc = conjunction_scorer.next_doc();
    }
    ASSERT_EQ(result, expect_result);
}