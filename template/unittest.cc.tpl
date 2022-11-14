#include <stdio.h>
#include <memory>
#include "gtest/gtest.h"
#include "unittests/test_utils.h"

#define private public
#define protected public
#include "{{header}}"
#undef private
#undef protected

using namespace std;
using namespace vsso;
using namespace test_utils;

class {{class}} : public ::testing::Test {
private:
    virtual void SetUp() {
        //p_obj.reset(new {{source_class}});
    }
    virtual void TearDown() {
        //if (p_obj != nullptr) p_obj.reset();
    }
protected:
    //std::shared_ptr<{{source_class}}>  p_obj;
};

TEST_F({{class}}, init) {
    fprintf(stderr, ">>> {{class}} start...\n");
    ASSERT_EQ(1, 1);
    ASSERT_TRUE(1 == 1);
}
