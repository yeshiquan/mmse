#pragma once

#include <stdint.h>
#include <iostream>

namespace mmse {

// shared_ptr底层使用atomic来存储计数器，好处是线程安全，缺点是开销比较大
// 有一些场景并不需要保证线程安全，例如一次Query检索中构造的查询语法树，这个时候用shared_ptr就不太合适了
// RefPtr是一个简单的基于引用计数的智能指针，功能和shared_ptr类似，但是底层使用非atomic变量来存储计数器
// 它是线程不安全的，好处是开销比较小

using Counter = uint32_t;  // 线程不安全，效率高
//using Counter = std::atomic<int>; // 线程安全，效率低，等价于std::shared_ptr

template<typename T>
class RefPtr {
public:
    T* ptr {nullptr};
    // Counter count; 不能用普通变量来保存计数
    // 原因1：因为const对象无法修改成员变量
    // 原因2：多个RefPtr对象的count是共享的，如果用普通的成员变量，无法共享
    Counter *count{nullptr}; //办法是用指针

public:
    //构造
    explicit RefPtr(T* p = nullptr) noexcept : ptr(p) {
        count = new Counter;
        if(ptr != nullptr) {
            *count = 1;
        }
    }
    RefPtr(std::nullptr_t n) noexcept {}
    void reset(T* p = nullptr) {
        if (--*count == 0 && ptr) {
            delete ptr;
        }
        ptr = p;
        if (ptr != nullptr) {
            *count = 1;
        }
    }
    T* get() noexcept { return ptr; }
    const T* get() const noexcept { return ptr; }
    //拷贝构造
    RefPtr(const RefPtr& r) noexcept {
        ptr = r.ptr; //让两个指针指向同一个地方
        count = r.count; //让count指针和原来的指向同一个地方
        ++*r.count; //原来的对象的引用计数++
    }
    template<typename U>
    RefPtr(const RefPtr<U>& r) noexcept {
        ptr = r.ptr;
        count = r.count;
        ++*r.count;
    }    
    //移动构造
    RefPtr(RefPtr&& r) noexcept {
        ptr = r.ptr;
        count = r.count;
        r.ptr = nullptr;
        r.count = nullptr;
    }
    template<typename U>
    RefPtr(RefPtr<U>&& r) noexcept {
        ptr = r.ptr;
        count = r.count;
        r.ptr = nullptr;
        r.count = nullptr;
    }
    void swap(RefPtr& other) {
        std::swap(ptr, other.ptr);
        std::swap(count, other.count);
    }
    RefPtr& operator=(std::nullptr_t nt) noexcept {
        return *this;
    }    
    RefPtr& operator=(const RefPtr& r) noexcept {
        RefPtr tmp(r);
        swap(tmp);
        return *this;
    }
    // 用于子类的RefPtr赋值给父类的RefPtr
    // 例如 RefPtr<Base> ptr = RefPtr<Derived>();
    template<typename U>
    RefPtr& operator=(const RefPtr<U>& r) noexcept {
        RefPtr tmp(r);
        swap(tmp);
        return *this;
    }
    RefPtr& operator=(RefPtr&& r) noexcept {
        RefPtr tmp(std::move(r));
        swap(tmp);
        return *this;
    }    
    template<typename U>
    RefPtr& operator=(RefPtr<U>&& r) noexcept {
        RefPtr tmp(std::move(r));
        swap(tmp);
        return *this;
    }
    operator bool() noexcept {
        return ptr != nullptr;
    }
    T& operator*() noexcept {
        return *ptr;
    }
    const T& operator*() const noexcept {
        return *ptr;
    }    
    T* operator->() noexcept {
        return ptr;
    }    
    const T* operator->() const noexcept {
        return ptr;
    }        
    //析构
    ~RefPtr() noexcept {
        if(count && (--*count) == 0) {
            delete ptr;
            delete count;
        }
    }
    //引用计数
    int use_count() noexcept {
        return *count;
    }
    void ref(T* p) {
        RefPtr tmp(p);
        swap(tmp);
    }
};

template <typename T, typename... Args>
RefPtr<T> make(Args&&... args) {
    return RefPtr<T>(new T(std::forward<Args>(args)...));
}

} // namespace