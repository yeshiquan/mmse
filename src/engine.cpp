#include "engine.h"

namespace mmse {

Engine::Engine() {
    _segment = new SegmentInfo;
}

Engine::~Engine() {
    std::cout << "Destroy Engine" << std::endl;
    delete _segment;
}

} // namespace