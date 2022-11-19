#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <random>

#define  DCHECK_IS_ON
#define POOL_STATS true

#define private public
#define protected public
#include "index_writer.h"
#undef private
#undef protected

#include "index/document.h"
#include "index/segment_info.h"
#include "engine.h"

using namespace unise;


class IndexWriterTest : public ::testing::Test {
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

TEST_F(IndexWriterTest, test_basic) {
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
        doc->add_field(new TextField(0, doc_text[i], FieldStoreType::YES));
        doc->add_field(new NumberField(1, i*2, FieldStoreType::YES));
        doc->add_field(new StringField(2, "hello", FieldStoreType::YES));
        index_writer.add_document(doc);
    }
    index_writer.build();

    TermId term_a = gen_term_id("a");
    TermId term_g = gen_term_id("g");

    std::cout << "term_a:" << term_a << std::endl;
    std::cout << "term_g:" << term_g << std::endl;

    SegmentInfo* seg = Engine::instance().get_segment();
    const PostingList* list_a = seg->find(term_a);
    const PostingList* list_g = seg->find(term_g);
    ASSERT_TRUE(list_a != nullptr);
    ASSERT_TRUE(list_g != nullptr);

    auto print_posting_list = [](std::string name, const PostingList* list) {
        if (list == nullptr) {
            return;
        }
        std::cout << "list " << name << " -> [";
        const SkipList<DocId>& skiplist = list->get_skip_list();
        for (auto iter = skiplist.cbegin(); iter != skiplist.cend(); ++iter) {
            std::cout << *iter << ",";
        }
        std::cout << "]" << std::endl;
    };

    print_posting_list("list a", list_a);
    print_posting_list("list g", list_g);
}