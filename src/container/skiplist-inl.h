#pragma once

#include <algorithm>
#include <atomic>
#include <limits>
#include <iostream>
#include <memory>
#include <type_traits>
#include <cmath>
#include <assert.h>
#include <random>

namespace mmse::detail {

template<typename T>
class SkipListNode {
public:
    using ValueType = T;

    template<typename NodeAlloc, typename U>
    static SkipListNode* create(NodeAlloc& alloc, uint8_t height, U&& data) {
        size_t bytes = sizeof(SkipListNode) + height * sizeof(SkipListNode*);
        auto* node = (SkipListNode*)alloc.allocate(bytes);
        new(node) SkipListNode(height, std::forward<U>(data));
        return node;
    }

    template<typename NodeAlloc>
    static void destroy(NodeAlloc& alloc, SkipListNode* node) {
        size_t bytes = sizeof(SkipListNode) + node->height_ * sizeof(SkipListNode*);
        node->~SkipListNode();
        alloc.deallocate((uint8_t*)node, bytes);        
    }    

    template<typename U>
    SkipListNode(uint8_t height, U&& data) 
                : height_(height)
                , data_(std::forward<U>(data)){
        for (uint8_t i = 0; i < height; ++i) {
            skip_[i] = nullptr;
        }
    }

    ~SkipListNode() {
    }

    void copy_skip_pointer(SkipListNode* old_head) {
        for (uint8_t i = 0; i < old_head->height_; ++i) {
            set_skip(i, old_head->skip(i));
        }
    }

    uint8_t height() const { return height_; }
    const ValueType& data() const { return data_; }
    ValueType& data() { return data_; }
    SkipListNode* skip(int i) const { 
        assert(i < height_);
        return skip_[i]; 
    }
    void set_skip(int i, SkipListNode* node) {
        assert(i < height_);
        skip_[i] = node; 
    }
private:
    uint8_t height_{0};
    ValueType data_;
    SkipListNode* skip_[0];
};

class SkipListRandomHeight {
  enum { kMaxHeight = 64 };
 public:
  // make it a singleton.
  static SkipListRandomHeight &instance() {
    static SkipListRandomHeight instance_;
    return instance_;
  }

  int getHeight(int maxHeight) const {
    //DCHECK_LE(maxHeight, kMaxHeight) << "max height too big!";
    double p = randomProb();
    for (int i = 0; i < maxHeight; ++i) {
      if (p < lookupTable_[i]) {
        return i + 1;
      }
    }
    return maxHeight;
  }

  size_t getSizeLimit(int height) const {
    //DCHECK_LT(height, kMaxHeight);
    return sizeLimitTable_[height];
  }

 private:
  SkipListRandomHeight() { initLookupTable(); }

  void initLookupTable() {
    // set skip prob = 1/E
    static const double kProbInv = exp(1);
    static const double kProb = 1.0 / kProbInv;
    static const size_t kMaxSizeLimit = std::numeric_limits<size_t>::max();

    double sizeLimit = 1;
    double p = lookupTable_[0] = (1 - kProb);
    sizeLimitTable_[0] = 1;
    for (int i = 1; i < kMaxHeight - 1; ++i) {
      p *= kProb;
      sizeLimit *= kProbInv;
      lookupTable_[i] = lookupTable_[i - 1] + p;
      sizeLimitTable_[i] = sizeLimit > kMaxSizeLimit ?
        kMaxSizeLimit :
        static_cast<size_t>(sizeLimit);
    }
    lookupTable_[kMaxHeight - 1] = 1;
    sizeLimitTable_[kMaxHeight - 1] = kMaxSizeLimit;
  }

  static double randomProb() {
    static thread_local std::mt19937 gen(123);
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    double x = dis(gen);    
    return x;
  }

  double lookupTable_[kMaxHeight];
  size_t sizeLimitTable_[kMaxHeight];
};


} // namespace