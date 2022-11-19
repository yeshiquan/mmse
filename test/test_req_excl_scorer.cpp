#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <random>

#define  DCHECK_IS_ON
#define POOL_STATS true

#define private public
#define protected public
#include "search/req_excl_scorer.h"
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

using namespace mmse;


class ReqExclScorerTest : public ::testing::Test {
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

#define BUILD_REQEXCL_SCORER() \
    PostingList list1(111111); \
    PostingList list2(222222); \
    auto build_list = [](PostingList& list, std::vector<DocId> docs) { \
        for (auto doc : docs) { \
            list.add_doc(doc); \
        } \
    }; \
    build_list(list1, {1,3,4,5,7,8,9,12,15,18,20,22});  \
    build_list(list2, {2,6,7,8,10,11,12}); \
    TermScorer t1(&list1); \
    TermScorer t2(&list2); \
    ReqExclScorer req_excl_scorer(&t1, &t2);

TEST_F(ReqExclScorerTest, test_next_doc) {
    BUILD_REQEXCL_SCORER();
    std::vector<DocId> result;
    std::vector<DocId> expect_result {1,3,4,5,9,15,18,20,22};
    DocId doc = req_excl_scorer.next_doc();
    while (doc != NO_MORE_DOCS) {
        std::cout << ">>>> req_excl doc -> " << doc << std::endl;
        result.emplace_back(doc);
        doc = req_excl_scorer.next_doc();
    }
    ASSERT_EQ(result, expect_result);
}

TEST_F(ReqExclScorerTest, test_skip_to) {
    BUILD_REQEXCL_SCORER();
    std::vector<DocId> result;

    DocId doc = req_excl_scorer.skip_to(1);
    ASSERT_EQ(doc, 1);
    ASSERT_EQ(req_excl_scorer.doc(), 1);

    doc = req_excl_scorer.skip_to(2);
    ASSERT_EQ(doc, 3);

    doc = req_excl_scorer.skip_to(19);
    ASSERT_EQ(doc, 20);

    doc = req_excl_scorer.skip_to(100);
    ASSERT_EQ(doc, NO_MORE_DOCS);
}