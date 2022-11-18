#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <queue>

#define  DCHECK_IS_ON
#define POOL_STATS true

class SmallTest : public ::testing::Test {
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

struct Scorer {
    Scorer(int d) : doc(d) {}
    int doc;
}; 
class ScorerLess {
public:
    bool operator() (const Scorer* a, const Scorer* b) {
        return a->doc > b->doc;
    }
};

TEST_F(SmallTest, test_basic) {
    Scorer* a = new Scorer(44);
    Scorer* b = new Scorer(55);
    Scorer* c = new Scorer(33);
    std::priority_queue<Scorer*, std::vector<Scorer*>, ScorerLess> queue; 
    queue.emplace(a);
    queue.emplace(b);
    queue.emplace(c);
    while (!queue.empty()) {
        std::cout << queue.top()->doc << std::endl;
        queue.pop();
    }
}