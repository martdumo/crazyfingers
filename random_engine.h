#ifndef RANDOM_ENGINE_H
#define RANDOM_ENGINE_H

#include <vector>
#include <random>

namespace Guitar {

// ============================================================================
// Random Engine - Weighted Random Selection
// ============================================================================

class RandomEngine {
public:
    RandomEngine();

    [[nodiscard]] int generateInt(int min_val, int max_val);
    [[nodiscard]] bool generateBool();

    // Select index based on weights using discrete_distribution
    template<typename WeightType>
    [[nodiscard]] int selectWeighted(const std::vector<WeightType>& weights);

private:
    std::mt19937 engine_;
};

// ============================================================================
// Template Implementation
// ============================================================================

template<typename WeightType>
int RandomEngine::selectWeighted(const std::vector<WeightType>& weights) {
    if (weights.empty()) return -1;
    if (weights.size() == 1) return 0;

    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    return dist(engine_);
}

} // namespace Guitar

#endif // RANDOM_ENGINE_H
