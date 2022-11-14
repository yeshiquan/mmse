#pragma once

#include "segment_info.h"

namespace unise {
 
class Engine {
public:
    ~Engine();
    static Engine& instance() {
        static Engine ins;
        return ins;
    }

    Engine(Engine const&) = delete;             // Copy construct
    Engine(Engine&&) = delete;                  // Move construct
    Engine& operator=(Engine const&) = delete;  // Copy assign
    Engine& operator=(Engine &&) = delete;      // Move assign    

    SegmentInfo* get_segment() { return _segment; }

private:
    Engine();
    SegmentInfo* _segment = nullptr;
};

} // namespace