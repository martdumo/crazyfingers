#include "random_engine.h"

namespace Guitar {

// ============================================================================
// RandomEngine Implementation
// ============================================================================

RandomEngine::RandomEngine() : engine_{std::random_device{}()} {}

int RandomEngine::generateInt(int min_val, int max_val) {
    std::uniform_int_distribution<> dist(min_val, max_val);
    return dist(engine_);
}

bool RandomEngine::generateBool() {
    std::uniform_int_distribution<> dist(0, 1);
    return dist(engine_) == 0;
}

// Explicit template instantiation for int weights
template int RandomEngine::selectWeighted<int>(const std::vector<int>&);

} // namespace Guitar
