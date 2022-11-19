#include <gtest/gtest.h>
#include "ref_ptr.h"

#define  DCHECK_IS_ON
#define POOL_STATS true

using namespace mmse;

class RefPtrTest : public ::testing::Test {
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

TEST_F(RefPtrTest, test_basic) {
    static int instance_cnt = 0;
    struct Foo {
        Foo(double dv) {
            v = dv;
            instance_cnt++;
            std::cout << "Foo()\n";
        }
        ~Foo() {
            instance_cnt--;
            std::cout << "~Foo()\n";
        }
        double v;
    };

    {
        RefPtr<Foo> ptr0(new Foo(3.14));
        ASSERT_EQ((*ptr0).v, 3.14);
        RefPtr<Foo> ptr1 = ptr0;
        ASSERT_EQ((*ptr1).v, 3.14);
        ASSERT_EQ(ptr0.use_count(), ptr1.use_count());
        ASSERT_EQ(ptr0.use_count(), 2);

        RefPtr<Foo> ptr2(new Foo(1.59));
        RefPtr<Foo> ptr3 = ptr2;
        ASSERT_EQ(ptr2.use_count(), 2);
        ASSERT_EQ(ptr3.use_count(), 2);
        ptr3 = ptr1;
        ASSERT_EQ(ptr2.use_count(), 1);
        ASSERT_EQ(ptr1.use_count(), 3);

        RefPtr<Foo> ptr4 = std::move(ptr1);
        ASSERT_EQ(ptr4.use_count(), 3);
        ASSERT_EQ(ptr1.get(), nullptr);
    }
    // 所有实例都被自动回收
    ASSERT_EQ(instance_cnt, 0);
}

TEST_F(RefPtrTest, test_derived) {
    struct Foo {};
    struct Bar : public Foo {};
    RefPtr<Foo> ptr = mmse::make<Bar>();
}

TEST_F(RefPtrTest, test_bool) {
    RefPtr<std::string> empty_str;
    int stat_cnt = 0;
    if (empty_str) {
        stat_cnt++;
    }
    ASSERT_EQ(stat_cnt, 0);
    empty_str.ref(new std::string("hello"));
    if (empty_str) {
        stat_cnt++;
    }
    ASSERT_EQ(stat_cnt, 1);

    RefPtr<std::string> tmp(nullptr); // 可以通过nullptr来构造
}

TEST_F(RefPtrTest, test_reset) {
    static int instance_cnt = 0;
    struct Foo {
        Foo(double dv) {
            v = dv;
            instance_cnt++;
        }
        ~Foo() {
            instance_cnt--;
        }
        double v;
    };
    {
        RefPtr<Foo> ptr1(new Foo(3.14));
        ASSERT_EQ(ptr1.use_count(), 1);
        ASSERT_EQ(instance_cnt, 1);
        ptr1.reset(new Foo(2.14));
        ASSERT_EQ(ptr1.use_count(), 1);
        ASSERT_EQ(instance_cnt, 1);
    }
    // 所有实例都被自动回收
    ASSERT_EQ(instance_cnt, 0);    
}