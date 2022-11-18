#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <random>

#define  DCHECK_IS_ON
#define POOL_STATS true

#define private public
#define protected public
#include "search/disjunction_scorer.h"
#undef private
#undef protected
#include "defer.h"

#include "index/document.h"
#include "engine.h"
#include "index/segment_info.h"
#include "search/query.h"
#include "search/boolean_query.h"
#include "search/term_query.h"
#include "search/term_scorer.h"

using namespace unise;


class DisjunctionScorerTest : public ::testing::Test {
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

#define BUILD_DISJUNCTION_SCORER() \
    PostingList list1(111111); \
    PostingList list2(222222); \
    PostingList list3(333333); \
    auto build_list = [](PostingList& list, std::vector<DocId> docs) { \
        for (auto doc : docs) { \
            list.add_doc(doc); \
        } \
    }; \
    build_list(list1, {2,13,17,20,98});  \
    build_list(list2, {1,13,22,35,98,99}); \
    build_list(list3, {1,3,13,20,35,80,98}); \
    TermScorer t1(&list1); \
    TermScorer t2(&list2); \
    TermScorer t3(&list3); \
    std::vector<Scorer*> requred_scorers = {&t1, &t2, &t3}; \
    DisjunctionScorer disjunction_scorer(requred_scorers, 2);

TEST_F(DisjunctionScorerTest, test_next_doc) {
    BUILD_DISJUNCTION_SCORER();
    std::vector<DocId> result;
    std::vector<DocId> expect_result {1, 13, 20, 35, 98};
    DocId doc = disjunction_scorer.next_doc();
    while (doc != NO_MORE_DOCS) {
        std::cout << ">>>> disjunction doc -> " << doc << std::endl;
        result.emplace_back(doc);
        doc = disjunction_scorer.next_doc();
    }
    ASSERT_EQ(result, expect_result);
}

TEST_F(DisjunctionScorerTest, test_skip_to) {
    BUILD_DISJUNCTION_SCORER();
    std::vector<DocId> result;
    DocId doc = disjunction_scorer.skip_to(3);
    ASSERT_EQ(doc, 13);
    ASSERT_EQ(disjunction_scorer.doc(), 13);

    doc = disjunction_scorer.skip_to(20);
    ASSERT_EQ(doc, 20);

    doc = disjunction_scorer.skip_to(90);
    ASSERT_EQ(doc, 98);

    doc = disjunction_scorer.skip_to(100);
    ASSERT_EQ(doc, NO_MORE_DOCS);
}