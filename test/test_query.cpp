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

#include "document.h"
#include "engine.h"
#include "segment_info.h"
#include "index_writer.h"

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
        "a b c d e f g a a a b c",
        "a b c d e f g a b c",
        "a b c d e f g a a a",
        "c d e",
        "c d e",
        "f g",
        "f g",
        "f g",
        "f g",
        "g"
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

TEST_F(QueryTest, test_basic) {
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