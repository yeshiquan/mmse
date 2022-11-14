#pragma once

#include <algorithm>
#include <atomic>
#include <limits>
#include <iostream>
#include <memory>
#include <tuple>
#include <type_traits>

#include "skiplist-inl.h"

namespace unise {

template<typename T, 
         typename Comp = std::less<T>,
         typename NodeAlloc = std::allocator<uint8_t>, 
         uint32_t MAX_HEIGHT = 24>
class SkipList {
    using SkipListType = SkipList<T, Comp, NodeAlloc, MAX_HEIGHT>;
    using NodeType = detail::SkipListNode<T> ;
    using ValueType = T;

    template<bool C>
    class Iterator {
    public:
        Iterator(NodeType* node) : _node(node) {}
        ~Iterator() {}

        const T& operator*() const {
            return _node->data();
        }
        const T* operator->() const {
            return &_node->data();
        }
        template<bool Z = C, typename = std::enable_if_t<(!Z)>>
        T& operator*() noexcept {
            return _node->data();
        }
        template<bool Z = C, typename = std::enable_if_t<(Z)>>
        const T& operator*() const noexcept {
            return _node->data();
        }
        template<bool Z = C, typename = std::enable_if_t<(!Z)>>
        T* operator->() noexcept {
            return &_node->data();
        }
        template<bool Z = C, typename = std::enable_if_t<(Z)>>
        const T* operator->() const noexcept {
            return &_node->data();
        }
        // 前置操作重载
        const Iterator& operator++() {
            _node = _node->skip(0);
            return *this;
        }
        // 后置操作重载
        Iterator operator++(int) {
            auto prev = *this;
            ++*this;
            return prev;
        }
        bool operator==(const Iterator& o) const {
            return _node == o._node;
        }
        bool operator!=(const Iterator& o) const {
            return !(*this == o);
        }
        Iterator& operator=(const Iterator& o) {
            Iterator tmp(o);
            swap(tmp);
            return *this;
        }

        Iterator(const Iterator& o) {
            _node = o._node;
        }

        Iterator(Iterator&& o) {
            _node = o._node;
        }
        void swap(Iterator& o) {
            std::swap(_node, o._node);
        }
    private:
        NodeType* _node = {nullptr};
    };    
public:
    SkipList(uint8_t init_height) : alloc_(NodeAlloc()) {
        head_ = NodeType::create(alloc_, init_height, ValueType());
    }
    SkipList() : alloc_(NodeAlloc()) {
        head_ = NodeType::create(alloc_, 8, ValueType());
    }    
    ~SkipList() {
        NodeType* node = head_;
        while (node) {
            NodeType* node_next = node->skip(0);
            NodeType::destroy(alloc_, node);
            node = node_next;
        }
    }

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    iterator begin() {
        return iterator(head_->skip(0));
    }
    iterator end() {
        return iterator(nullptr);
    }
    const_iterator cbegin() const {
        return const_iterator(head_->skip(0));
    }
    const_iterator cend() const {
        return const_iterator(nullptr);
    }

    bool insert(const ValueType& data) {
        return insert(data, false);
    }

    bool insert(const ValueType& data, bool is_force_insert) {
        int current_height = height();
        size_t newSize = size_ + 1;
        size_t sizeLimit = detail::SkipListRandomHeight::instance().getSizeLimit(current_height);
        if (current_height < MAX_HEIGHT && newSize > sizeLimit) {
            //std::cout << "grow_height() " << sizeLimit << " -> " << newSize << std::endl;
            grow_height(current_height + 1);
        }        
        int node_height = detail::SkipListRandomHeight::instance().getHeight(height());
        //std::cout<< "insert() node_height:" << node_height << " data:" << data << " list_height:" << height() << std::endl;

        std::vector<NodeType*> prevs(height());
        auto* next_node = _search_equal_or_greater(data, prevs);
        bool is_exist = next_node && next_node->data() == data;

        if ((is_exist && is_force_insert) || !is_exist) {
            auto* new_node = NodeType::create(alloc_, node_height, data);
            for (int layer = node_height - 1; layer >= 0; layer--) {
                new_node->set_skip(layer, prevs[layer]->skip(layer));
                prevs[layer]->set_skip(layer, new_node);
            }
            size_++;
        }
        return is_exist;
    }

    bool remove(const ValueType& data) {
        //std::cout << "remove(" << data << ")" << std::endl;
        int layer = height() - 1;
        NodeType* node = head_;
        NodeType* node_next = nullptr;

        std::vector<NodeType*> prevs(height());
        auto* to_delete_node = _search_equal_or_greater(data, prevs);

        if (to_delete_node == nullptr || to_delete_node->data() != data) {
            return false;  
        }

        for (int l = to_delete_node->height() - 1; l >= 0; --l) {
            prevs[l]->set_skip(l, to_delete_node->skip(l));
        }

        NodeType::destroy(alloc_, to_delete_node);
        size_--;
        return true;
    }

    void grow_height(int new_height) {
        if (head_->height() >= new_height) {
            // 别人做了grow
            return;
        }
        NodeType* old_head = head_;
        NodeType* new_head = NodeType::create(alloc_, new_height, ValueType());
        new_head->copy_skip_pointer(old_head);
        head_ = new_head;
        NodeType::destroy(alloc_, old_head);
    }

    bool find(const ValueType& data) {
        auto* node = _search_equal_or_greater(data);
        return node != nullptr && node->data() == data;        
    }

    NodeType* advance(const ValueType& data) {
        return _search_equal_or_greater(data);
    }    

  void print_layer(NodeType* start, int layer, int& n) {
      NodeType* node = start;
      std::cout << "layer[" << layer << "] -> ";
      while (node) {
          std::cout << node->data() << ",";
          n++;
          node = node->skip(layer);
      }
      std::cout << std::endl;
  }

  void print() {
      printf("\n============ SkipList print begin(%zu) ================\n", size_);
      int n = 0;
      auto top = maxLayer();
      for (int layer = top; layer >= 0; --layer) {
          n = 0;
          print_layer(head_->skip(layer), layer, n);
      }
      printf("============ SkipList print end(%d) ================\n\n", n);
      if (size_ != n) {
          printf("ERROR elements size invalid\n");
      }
  }
  int height() const { return head_->height(); }
  int maxLayer() const { return height() - 1; }  
  int size() const { return size_; }
private:
    // 寻找 node >= data的节点，同时返回前驱节点
    NodeType* _search_equal_or_greater(const ValueType& data, 
                                            std::vector<NodeType*>& prevs) {
        NodeType* cur = head_;
        NodeType* next = nullptr;
        for(int i = maxLayer(); i >= 0; i--){
            next = cur->skip(i);
            while(next && less(next->data(), data)){
                // 一直寻找到刚好大于等于data的节点的前置节点
                cur = next;
                next = next->skip(i);
            }
            prevs[i] = cur;
        }
        return next;       
    }

    // 寻找 node >= data 的节点
    NodeType* _search_equal_or_greater(const ValueType& data) {
        NodeType* cur = head_;
        NodeType* next = nullptr;
        for(int i = maxLayer(); i >= 0; i--){
            next = cur->skip(i);
            while(next && less(next->data(), data)){
                // 一直寻找到刚好大于等于data的节点的前置节点
                cur = next;
                next = next->skip(i);
            }
        }
        return next;
    }        

   // // 容易导致性能恶化的实现
    // bool find_insert_point(NodeType* new_node, const ValueType& data) {
    //     NodeType* node = head_;
    //     NodeType* node_next = nullptr;
    //     std::vector<NodeType*> prevs(new_node->height());

    //     int layer = new_node->height() - 1;  // Bad
    //     int layer = maxLayer();              // Good
    //     // layer如果从新节点开始初始化，会导致性能恶化，
    //     // 因为新节点的高度可能很小，这样会从中间某一层节点从头开始搜索，中间这层节点数很多的时候，性能恶化
    //     // 永远要从最顶层开始搜索，才能充分利用跳表的索引
    //     // 永远不要从中间或者最底层节点搜索，性能会退化很严重
    //     for (; layer >= 0; --layer) {
    //         node_next = node->skip(layer);
    //         while (node_next && (data > node_next->data())) {
    //             node = node_next;
    //             node_next = node_next->skip(layer);
    //         }
    //         prevs[layer] = node;
    //     }
    //     for (int layer = new_node->height() - 1; layer >= 0; layer--) {
    //         new_node->set_skip(layer, prevs[layer]->skip(layer));
    //         prevs[layer]->set_skip(layer, new_node);
    //     }        
    //     bool is_exist = new_node->skip(0) && new_node->skip(0)->data() == data;
 
    //     size_++;  
    //     return is_exist;
    // }        

  static bool greater(const ValueType &data1, const ValueType& data2) {
    return Comp()(data2, data1);
  }
  static bool less(const ValueType &data1, const ValueType& data2) {
    return Comp()(data1, data2);
  }
  static bool greater_equal(const ValueType &data1, const ValueType& data2) {
    return !Comp()(data1, data2);
  }
  static bool less_equal(const ValueType &data1, const ValueType& data2) {
    return !Comp()(data2, data1);
  }  
  static bool equal(const ValueType &data1, const ValueType& data2) {
    return !Comp()(data2, data1) && !Comp()(data1, data2);
  }    
private:
    NodeType* head_{nullptr};
    size_t size_{0};
    NodeAlloc alloc_;
};

} // namespace