#include "easter_egg.h"
#include <sstream>

namespace EasterEgg {

// ============================================================================
// Implementation
// ============================================================================

std::string generateAbsurdFact() {
    // Initialize random engine
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Select random indices for each array
    std::uniform_int_distribution<> subject_dist(0, NUM_SUBJECTS - 1);
    std::uniform_int_distribution<> action_dist(0, NUM_ACTIONS - 1);
    std::uniform_int_distribution<> reason_dist(0, NUM_REASONS - 1);
    
    int subject_idx = subject_dist(gen);
    int action_idx = action_dist(gen);
    int reason_idx = reason_dist(gen);
    
    // Build the absurd fact
    std::ostringstream oss;
    oss << "Sabia usted que "
        << SUBJECTS[subject_idx] << " "
        << ACTIONS[action_idx] << " "
        << REASONS[reason_idx] << "?";
    
    return oss.str();
}

} // namespace EasterEgg
