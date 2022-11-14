#include "engine.h"

namespace unise {

Engine::Engine() {
    _segment = new SegmentInfo;
}

Engine::~Engine() {
    std::cout << "Destroy Engine" << std::endl;
    delete _segment;
}

} // namespace