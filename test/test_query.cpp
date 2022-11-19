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

using namespace unise;


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
        doc->add_field(new TextField(0, doc_text[i], YES));
        doc->add_field(new NumberField(1, i*2, YES));
        doc->add_field(new StringField(2, "hello", YES));
        index_writer.add_document(doc);
    }
    index_writer.build();
}

TEST_F(QueryTest, test_basic1) {
    build_index();

    Query* query1 = new TermQuery(Term("content", "f"));
    Query* query2 = new TermQuery(Term("content", "c"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

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

    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

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

    Query* query1 = new TermQuery(Term("content", "z"));
    Query* query2 = new TermQuery(Term("content", "e"));
    Query* query3 = new TermQuery(Term("content", "c"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::SHOULD);
    query->set_min_should_match(1);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

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

    Query* query1 = new TermQuery(Term("content", "z"));
    Query* query2 = new TermQuery(Term("content", "e"));
    Query* query3 = new TermQuery(Term("content", "c"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::SHOULD);
    query->set_min_should_match(0);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

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
    Query* query1 = new TermQuery(Term("content", "a"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case01) {
    Query* query1 = new TermQuery(Term("content", "a"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST_NOT);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case1) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(3);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case2) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(2);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case3) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(0);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case4) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(2);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case5) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::SHOULD);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(2);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case6) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::SHOULD);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(3);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case61) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::SHOULD);
    query->add(query2, Occur::SHOULD);
    query->add(query3, Occur::MUST_NOT);
    query->add(query4, Occur::MUST_NOT);
    query->add(query5, Occur::MUST_NOT);
    query->set_min_should_match(1);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case7) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(3);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case8) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    query->add(query3, Occur::SHOULD);
    query->add(query4, Occur::SHOULD);
    query->add(query5, Occur::SHOULD);
    query->set_min_should_match(2);
    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}

TEST_F(QueryTest, test_boolean_query_case9) {
    Query* query1 = new TermQuery(Term("content", "a"));
    Query* query2 = new TermQuery(Term("content", "g"));
    Query* query3 = new TermQuery(Term("content", "f"));
    Query* query4 = new TermQuery(Term("content", "k"));
    Query* query5 = new TermQuery(Term("content", "b"));
    Query* query6 = new TermQuery(Term("content", "z"));
    BooleanQuery* query = new BooleanQuery;
    query->add(query1, Occur::MUST);
    query->add(query2, Occur::MUST_NOT);
    query->add(query3, Occur::SHOULD);
    query->set_min_should_match(1);

    BooleanQuery* sub_query = new BooleanQuery;
    sub_query->add(query4, Occur::MUST);
    sub_query->add(query5, Occur::MUST_NOT);
    sub_query->add(query6, Occur::SHOULD);    
    query->add(sub_query, Occur::MUST);

    Weight* weight = query->create_weight();
    Scorer* scorer = weight->make_scorer();

    for (auto& line : scorer->explain()) {
        std::cout << line << std::endl;
    }
}