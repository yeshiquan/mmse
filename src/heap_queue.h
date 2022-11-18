#pragma once
#include <vector>
namespace unise {
 
template<typename T, typename Comp = std::less<T>>
class HeapQueue {
public:
    HeapQueue() {}
    ~HeapQueue() {}

    // 用于插入新元素
    void _adjust_up(T* array, int size, int i) {
        if (i == 0) {
            return;
        }
        int parent = (i-1) / 2;
        if (less(array[i], array[parent])) {
            std::swap(array[i], array[parent]);
            _adjust_up(array, size, parent);
        }
    }

    // 用于建堆、替换堆顶、删除堆顶的操作
    void _adjust_down(T* array, int size, int i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int small = i;
        if (left < size) {
            if (greater(array[small], array[left])) {
                small = left;
            }
        }
        if (right < size) {
            if (greater(array[small], array[right])) {
                small = right;
            }
        }
        if (small != i) {
            std::swap(array[i], array[small]);
            _adjust_down(array, size, small); //递归法简单不容易出错
        }
    }

    void _build_heap(T* array, int size) {
        for (int i = size/2-1; i >= 0; --i) {
            _adjust_down(array, size, i);
        }
    }

    void adjust_all() {
        _build_heap(_elements.data(), _elements.size());
    }

    void adjust_down_from_top() {
        _adjust_down(_elements.data(), _elements.size(), 0);
    }

    void pop() {
        std::swap(_elements[0], _elements[_elements.size() - 1]);
        _elements.pop_back();
        _adjust_down(_elements.data(), _elements.size(), 0);
    }

    void push(const T& data) {
        _elements.emplace_back(data);
        _adjust_up(_elements.data(), _elements.size(), _elements.size()-1);
    }

    const T& top() const {
        return _elements[0];
    }

    bool empty() const {
        return _elements.empty();
    }
    size_t size() const {
        return _elements.size();
    }
    static bool greater(const T &data1, const T& data2) {
        return Comp()(data2, data1);
    }
    static bool less(const T &data1, const T& data2) {
        return Comp()(data1, data2);
    }
    static bool greater_equal(const T &data1, const T& data2) {
        return !Comp()(data1, data2);
    }
    static bool less_equal(const T &data1, const T& data2) {
        return !Comp()(data2, data1);
    }  
    static bool equal(const T &data1, const T& data2) {
        return !Comp()(data2, data1) && !Comp()(data1, data2);
    }

    typename std::vector<T>::iterator begin() { return _elements.begin(); }
    typename std::vector<T>::iterator end() { return _elements.end(); }
    typename std::vector<T>::const_iterator cbegin() const { return _elements.cbegin(); }
    typename std::vector<T>::const_iterator cend() const { return _elements.cend(); }    
private:
    std::vector<T> _elements;
};

} // namespace