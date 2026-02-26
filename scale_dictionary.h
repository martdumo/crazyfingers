#ifndef SCALE_DICTIONARY_H
#define SCALE_DICTIONARY_H

#include <string>
#include <vector>
#include <unordered_map>

namespace Music {

// ============================================================================
// Scale Dictionary Class - Provides runtime access to all scales
// Over 70 scales from ethnomusicology consensus
// ============================================================================

class ScaleDictionary {
public:
    static ScaleDictionary& getInstance();
    
    // Get scale intervals by name
    [[nodiscard]] std::vector<int> getIntervals(const std::string& name) const;
    
    // Get all scale names
    [[nodiscard]] std::vector<std::string> getAllScaleNames() const;
    
    // Get scale names by category
    [[nodiscard]] std::vector<std::string> getScalesByCategory(const std::string& category) const;
    
    // Check if scale exists
    [[nodiscard]] bool hasScale(const std::string& name) const;
    
    // Get random scale name
    [[nodiscard]] std::string getRandomScaleName() const;

private:
    ScaleDictionary();
    void initializeDictionary();
    
    std::unordered_map<std::string, std::vector<int>> scales_;
    std::vector<std::string> all_names_;
    std::unordered_map<std::string, std::vector<std::string>> categories_;
};

} // namespace Music

#endif // SCALE_DICTIONARY_H
