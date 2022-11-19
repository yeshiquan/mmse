#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <random>

#define  DCHECK_IS_ON
#define POOL_STATS true

#define private public
#define protected public
#include "container/skiplist.h"
#undef private
#undef protected


class SkipListTest : public ::testing::Test {
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

TEST_F(SkipListTest, test_basic) {
    auto* sl = new mmse::SkipList<int>(4);
    //auto* sl = new mmse::SkipList<int>(8);
    std::vector<int> elements = {7, 14, 21, 32, 37, 71, 85, 119};

    // for (int i = 1; i <= 100; ++i) {
    //     elements.emplace_back(i);
    // }

    int not_exist_value = 120;
    bool ret = sl->find(not_exist_value);
    ASSERT_EQ(ret, false);
    sl->remove(not_exist_value);

    for (auto e : elements) {
        bool is_exist = sl->insert(e);
        ASSERT_EQ(is_exist, false);
        //插入重复元素
        is_exist = sl->insert(e);
        ASSERT_EQ(is_exist, true);

        sl->print();
        ret = sl->find(e);
        ASSERT_EQ(ret, true);
        ret = sl->find(not_exist_value);
        ASSERT_EQ(ret, false);
    }

    sl->print();

    for (auto e : elements) {
        bool ret = sl->find(e);
        ASSERT_EQ(ret, true);

        sl->remove(e);
        sl->print();
        // 删除后就找不到了
        ret = sl->find(e);
        ASSERT_EQ(ret, false);
    }
}

TEST_F(SkipListTest, test_simple_iterator) {
    int N = 200;
    int init_height = std::log2(100);
    auto* sl = new mmse::SkipList<int>(init_height);
    std::vector<int> elements;

    for (int i = 1; i <= N; ++i) {
        elements.emplace_back(i);
    }

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(elements), std::end(elements), rng);    

    for (auto e : elements) {
        bool is_exist = sl->insert(e);
        ASSERT_EQ(is_exist, false);
    }
    auto iterator = sl->get_simple_iterator();
    auto* node = iterator.next_node();
    std::cout << "simple iterator -> [";
    int n = 0;
    while (node) {
        std::cout << node->data() << ",";
        node = iterator.next_node();
        n++;
    }
    ASSERT_EQ(n, N);
    std::cout << "]" << std::endl;
}

TEST_F(SkipListTest, test_remove) {
    int N = 200;
    int init_height = std::log2(100);
    auto* sl = new mmse::SkipList<int>(init_height);
    std::vector<int> elements;

    for (int i = 1; i <= N; ++i) {
        elements.emplace_back(i);
    }

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(elements), std::end(elements), rng);    

    for (auto e : elements) {
        bool is_exist = sl->insert(e);
        ASSERT_EQ(is_exist, false);
    }

    sl->print();

    rng = std::default_random_engine {};
    std::shuffle(std::begin(elements), std::end(elements), rng);
    for (auto e : elements) {
        bool ret = sl->find(e);
        ASSERT_EQ(ret, true);

        sl->remove(e);

        ret = sl->find(e);
        ASSERT_EQ(ret, false);
    }
}

TEST_F(SkipListTest, test_grow_height) {
    int N = 425;
    int init_height = 2;
    auto* sl = new mmse::SkipList<int>(init_height);
    std::vector<int> elements;

    for (int i = 1; i <= N; ++i) {
        elements.emplace_back(i);
    }

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(elements), std::end(elements), rng);    

    for (auto e : elements) {
        bool is_exist = sl->insert(e);
        ASSERT_EQ(is_exist, false);
    }

    std::cout << "iterator value -> [";
    int cnt = 0;
    for (auto& v : *sl) {
        std::cout << v << ",";
        cnt++;
    }
    ASSERT_EQ(cnt, N);
    std::cout << "]" << std::endl;

    sl->print();

    // 高度增长了
    std::cout << "height " << init_height << " -> " << sl->height() << std::endl;
    ASSERT_GT(sl->height(), init_height);

    ASSERT_EQ(sl->size(), N);

    ASSERT_EQ(sl->find(5), true);
}