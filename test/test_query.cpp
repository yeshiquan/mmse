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
#undef private
#undef protected
#include "defer.h"

#include "index/document.h"
#include "index/segment_info.h"
#include "index_writer.h"
#include "engine.h"

using namespace mmse;


class QueryTest : public ::testing::Test {
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

void build_index() {
    IndexWriter index_writer;

    std::vector<std::string> doc_text = {
        "a b c d e f g a a a b c", // 0
        "a b c d e f g a b c", // 1
        "a b c d e f g a a a", // 2
        "c d e", // 3
        "c d e z", // 4
        "f z c", // 5
        "f g z", // 6
        "f g", // 7
        "f g", // 8
        "g" // 9
    };

    for (int i = 0; i < doc_text.size(); ++i) {
        auto doc = std::make_shared<Document>(i);
        doc->add_field(new TextField(0, doc_text[i], FieldStoreType::YES));
        doc->add_field(new NumberField(1, i*2, FieldStoreType::YES));
        doc->add_field(new StringField(2, "hello", FieldStoreType::YES));
        index_writer.add_document(doc);
    }
    index_writer.build();
}

TEST_F(QueryTest, test_basic1) {
    build_index();
    //RefPtr<Query> query1 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "c"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    std::vector<DocId> result;
    std::vector<DocId> expect_result{6,7,8};
    DocId doc = scorer->next_doc();
    while (doc != NO_MORE_DOCS) {
        std::cout << "Query a doc -> " << doc << std::endl;
        result.push_back(doc);
        doc = scorer->next_doc();
    }
    ASSERT_EQ(result, expect_result);
    //indexSearcher.search(query);
}

TEST_F(QueryTest, test_basic2) {
    build_index();

    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    std::vector<DocId> result;
    std::vector<DocId> expect_result{0, 1, 2};
    DocId doc = scorer->next_doc();
    while (doc != NO_MORE_DOCS) {
        std::cout << "Query a doc -> " << doc << std::endl;
        result.push_back(doc);
        doc = scorer->next_doc();
    }
    ASSERT_EQ(result, expect_result);
    //indexSearcher.search(query);
}

TEST_F(QueryTest, test_basic3) {
    build_index();

    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "z"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "e"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "c"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::SHOULD);
    query->set_min_should_match(1);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    std::vector<DocId> result;
    std::vector<DocId> expect_result{4, 5};
    DocId doc = scorer->next_doc();
    while (doc != NO_MORE_DOCS) {
        std::cout << "Query a doc -> " << doc << std::endl;
        result.push_back(doc);
        doc = scorer->next_doc();
    }
    ASSERT_EQ(result, expect_result);
    //indexSearcher.search(query);
}

TEST_F(QueryTest, test_basic4) {
    build_index();

    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "z"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "e"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "c"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::SHOULD);
    query->set_min_should_match(0);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    std::vector<DocId> result;
    std::vector<DocId> expect_result{4, 5, 6};
    DocId doc = scorer->next_doc();
    while (doc != NO_MORE_DOCS) {
        std::cout << "Query a doc -> " << doc << std::endl;
        result.push_back(doc);
        doc = scorer->next_doc();
    }
    ASSERT_EQ(result, expect_result);
    //indexSearcher.search(query);
}


TEST_F(QueryTest, test_boolean_query_case0) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case01) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST_NOT);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case1) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(3);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case2) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(2);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case3) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(0);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case4) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(2);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case5) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::SHOULD);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(2);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case6) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::SHOULD);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(3);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case61) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::SHOULD);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(1);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case7) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(3);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case8) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(2);
    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case9) {
    QueryPtr query1 = mmse::make<TermQuery>(Term("content", "a"));
    QueryPtr query2 = mmse::make<TermQuery>(Term("content", "g"));
    QueryPtr query3 = mmse::make<TermQuery>(Term("content", "f"));
    QueryPtr query4 = mmse::make<TermQuery>(Term("content", "k"));
    QueryPtr query5 = mmse::make<TermQuery>(Term("content", "b"));
    QueryPtr query6 = mmse::make<TermQuery>(Term("content", "z"));
    RefPtr<BooleanQuery> query = mmse::make<BooleanQuery>();
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    query->add(query3, Occur::SHOULD);
    query->set_min_should_match(1);

    RefPtr<BooleanQuery> sub_query = mmse::make<BooleanQuery>();
    sub_query->add(query4, Occur::MUST);
    sub_query->add(query5, Occur::MUST_NOT);
    sub_query->add(query6, Occur::SHOULD);    
    query->add(sub_query, Occur::MUST);

    WeightPtr weight = query->create_weight();
    ScorerPtr scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}