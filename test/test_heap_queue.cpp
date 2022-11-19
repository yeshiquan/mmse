#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <random>

#define  DCHECK_IS_ON
#define POOL_STATS true

#define private public
#define protected public
#include "container/heap_queue.h"
#undef private
#undef protected

using namespace mmse;


class HeapQueueTest : public ::testing::Test {
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

TEST_F(HeapQueueTest, test_basic) {
    HeapQueue<int, std::greater<int>> queue;
    for (int i = 1; i <= 5; ++i) {
        queue.push(i);
    }
    for (int i = 10; i > 5; --i) {
        queue.push(i);
    }
    while (!queue.empty()) {
        std::cout << "top element -> " << queue.top() << std::endl;
        queue.pop();
    }
}

TEST_F(HeapQueueTest, test_comp) {
    struct Scorer {
        Scorer(int d) : doc(d) {}
        int doc;           
    };
    class ScorerLess {
    public:
        bool operator() (const Scorer* a, const Scorer* b) {
            return a->doc < b->doc;
        }
    };    
    HeapQueue<Scorer*, ScorerLess> queue; 
    queue.push(new Scorer(3));
    queue.push(new Scorer(1));
    queue.push(new Scorer(4));
    queue.push(new Scorer(2));
    while (!queue.empty()) {
        std::cout << queue.top()->doc << std::endl;
        queue.pop();
    }    
}