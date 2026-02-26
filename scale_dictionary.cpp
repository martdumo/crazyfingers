#include "scale_dictionary.h"
#include <random>
#include <algorithm>

namespace Music {

// ============================================================================
// ScaleDictionary Implementation
// ============================================================================

ScaleDictionary::ScaleDictionary() {
    initializeDictionary();
}

ScaleDictionary& ScaleDictionary::getInstance() {
    static ScaleDictionary instance;
    return instance;
}

void ScaleDictionary::initializeDictionary() {
    // Common/Modes
    scales_["Major"] = {2, 2, 1, 2, 2, 2, 1};
    scales_["Harmonic Minor"] = {2, 1, 2, 2, 1, 3, 1};
    scales_["Melodic Minor"] = {2, 1, 2, 2, 2, 2, 1};
    scales_["Natural Minor"] = {2, 1, 2, 2, 1, 2, 2};
    scales_["Pentatonic Major"] = {2, 2, 3, 2, 3};
    scales_["Pentatonic Minor"] = {3, 2, 2, 3, 2};
    scales_["Pentatonic Blues"] = {3, 2, 1, 1, 3, 2};
    scales_["Pentatonic Neutral"] = {2, 3, 2, 3, 2};
    scales_["Ionian"] = {2, 2, 1, 2, 2, 2, 1};
    scales_["Dorian"] = {2, 1, 2, 2, 2, 1, 2};
    scales_["Phrygian"] = {1, 2, 2, 2, 1, 2, 2};
    scales_["Lydian"] = {2, 2, 2, 1, 2, 2, 1};
    scales_["Mixolydian"] = {2, 2, 1, 2, 2, 1, 2};
    scales_["Aeolian"] = {2, 1, 2, 2, 1, 2, 2};
    scales_["Locrian"] = {1, 2, 2, 1, 2, 2, 2};
    
    categories_["Common/Modes"] = {
        "Major", "Harmonic Minor", "Melodic Minor", "Natural Minor",
        "Pentatonic Major", "Pentatonic Minor", "Pentatonic Blues", "Pentatonic Neutral",
        "Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolian", "Locrian"
    };
    
    // Symmetric/Altered
    scales_["Chromatic"] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    scales_["Whole Tone"] = {2, 2, 2, 2, 2, 2};
    scales_["Octatonic (H-W)"] = {1, 2, 1, 2, 1, 2, 1, 2};
    scales_["Octatonic (W-H)"] = {2, 1, 2, 1, 2, 1, 2, 1};
    scales_["Augmented"] = {3, 1, 3, 1, 3, 1};
    scales_["Altered"] = {1, 1, 2, 2, 2, 2, 2};
    scales_["Diatonic"] = {2, 2, 1, 2, 2, 2, 1};
    scales_["Diminished"] = {2, 1, 2, 1, 2, 1, 2, 1};
    scales_["Diminished Half"] = {1, 2, 1, 2, 1, 2, 1, 2};
    scales_["Diminished Whole"] = {2, 1, 2, 1, 2, 1, 2, 1};
    scales_["Diminished Whole Tone"] = {1, 1, 1, 2, 2, 2, 3};
    scales_["Dominant 7th"] = {5, 2, 3, 2};
    scales_["Lydian Augmented"] = {2, 2, 2, 2, 1, 2, 1};
    scales_["Lydian Minor"] = {2, 2, 1, 1, 2, 2, 2};
    scales_["Lydian Diminished"] = {2, 2, 1, 1, 2, 2, 2};
    scales_["Half Diminished"] = {1, 2, 2, 1, 2, 2, 2};
    
    categories_["Symmetric/Altered"] = {
        "Chromatic", "Whole Tone", "Octatonic (H-W)", "Octatonic (W-H)",
        "Augmented", "Altered", "Diatonic", "Diminished", "Diminished Half",
        "Diminished Whole", "Diminished Whole Tone", "Dominant 7th",
        "Lydian Augmented", "Lydian Minor", "Lydian Diminished", "Half Diminished"
    };
    
    // Jazz/Bebop
    scales_["Bebop Major"] = {2, 2, 1, 2, 1, 1, 2, 2};
    scales_["Bebop Minor"] = {2, 1, 2, 2, 1, 1, 2, 2};
    scales_["Bebop Dominant"] = {2, 2, 1, 2, 2, 1, 1, 2};
    scales_["Bebop Half Diminished"] = {1, 2, 2, 1, 1, 2, 2, 2};
    scales_["Blues"] = {3, 2, 1, 1, 3, 2};
    scales_["Major Blues Scale"] = {2, 1, 1, 2, 3, 2};
    scales_["Dominant Pentatonic"] = {2, 2, 3, 2, 3};
    scales_["Mixo-Blues"] = {2, 2, 1, 2, 2, 3};
    
    categories_["Jazz/Bebop"] = {
        "Bebop Major", "Bebop Minor", "Bebop Dominant", "Bebop Half Diminished",
        "Blues", "Major Blues Scale", "Dominant Pentatonic", "Mixo-Blues"
    };
    
    // Exotic & World (A-J)
    scales_["Algerian"] = {2, 1, 3, 1, 1, 3, 1};
    scales_["Arabian #1"] = {2, 2, 1, 1, 2, 2, 2};
    scales_["Arabian #2"] = {1, 3, 1, 2, 1, 3, 1};
    scales_["Balinese"] = {1, 4, 1, 4, 2};
    scales_["Byzantine"] = {1, 3, 1, 2, 1, 3, 1};
    scales_["Chinese"] = {4, 2, 1, 4, 1};
    scales_["Chinese Mongolian"] = {2, 3, 2, 3, 2};
    scales_["Egyptian"] = {2, 3, 2, 3, 2};
    scales_["Eight Tone Spanish"] = {1, 2, 1, 2, 1, 2, 1, 2};
    scales_["Ethiopian (A raray)"] = {1, 2, 2, 2, 1, 2, 2};
    scales_["Ethiopian (Geez&Ezel)"] = {2, 1, 2, 2, 1, 2, 2};
    scales_["Hawaiian"] = {2, 3, 2, 3, 2};
    scales_["Hindu"] = {2, 2, 1, 2, 1, 2, 2};
    scales_["Hindustan"] = {2, 2, 1, 2, 2, 1, 2};
    scales_["Hirajoshi"] = {3, 1, 4, 1, 3};
    scales_["Hungarian Major"] = {3, 1, 1, 3, 1, 1, 2};
    scales_["Hungarian Gypsy"] = {2, 1, 3, 1, 1, 3, 1};
    scales_["Hungarian Minor"] = {2, 1, 3, 1, 1, 3, 1};
    scales_["Japanese #1"] = {1, 4, 2, 1, 4};
    scales_["Japanese #2"] = {2, 3, 2, 3, 2};
    scales_["Javaneese"] = {2, 2, 3, 2, 3};
    scales_["Jewish (Adonai Malakh)"] = {2, 2, 1, 2, 2, 1, 2};
    scales_["Jewish (Ahaba Rabba)"] = {1, 3, 1, 2, 1, 2, 2};
    
    // Exotic & World (K-Z)
    scales_["Kumoi"] = {2, 1, 4, 2, 3};
    scales_["Mohammedan"] = {2, 2, 1, 2, 2, 2, 1};
    scales_["Neopolitan"] = {1, 2, 2, 2, 2, 2, 1};
    scales_["Neopolitan Major"] = {1, 2, 2, 2, 2, 2, 1};
    scales_["Neopolitan Minor"] = {1, 2, 2, 2, 1, 3, 1};
    scales_["Oriental #1"] = {1, 3, 1, 1, 1, 3, 2};
    scales_["Oriental #2"] = {2, 1, 3, 1, 1, 2, 2};
    scales_["Pelog"] = {1, 2, 4, 1, 4};
    scales_["Persian"] = {1, 3, 1, 1, 1, 3, 2};
    scales_["Prometheus"] = {2, 2, 2, 3, 1, 2};
    scales_["Prometheus Neopolitan"] = {2, 2, 2, 3, 1, 2};
    scales_["Roumanian Minor"] = {2, 1, 3, 1, 1, 3, 1};
    scales_["Spanish Gypsy"] = {1, 3, 1, 2, 1, 2, 2};
    scales_["Super Locrian"] = {1, 1, 2, 2, 2, 2, 2};
    scales_["Iwato"] = {1, 4, 1, 4, 2};
    scales_["Moorish Phrygian"] = {1, 3, 1, 2, 1, 2, 2};
    scales_["Double Harmonic"] = {1, 3, 1, 2, 1, 3, 1};
    scales_["Enigmatic"] = {1, 3, 2, 2, 2, 1, 1};
    
    categories_["Exotic & World"] = {
        "Algerian", "Arabian #1", "Arabian #2", "Balinese", "Byzantine",
        "Chinese", "Chinese Mongolian", "Egyptian", "Eight Tone Spanish",
        "Ethiopian (A raray)", "Ethiopian (Geez&Ezel)", "Hawaiian", "Hindu",
        "Hindustan", "Hirajoshi", "Hungarian Major", "Hungarian Gypsy",
        "Hungarian Minor", "Japanese #1", "Japanese #2", "Javaneese",
        "Jewish (Adonai Malakh)", "Jewish (Ahaba Rabba)", "Kumoi", "Mohammedan",
        "Neopolitan", "Neopolitan Major", "Neopolitan Minor", "Oriental #1",
        "Oriental #2", "Pelog", "Persian", "Prometheus", "Prometheus Neopolitan",
        "Roumanian Minor", "Spanish Gypsy", "Super Locrian", "Iwato",
        "Moorish Phrygian", "Double Harmonic", "Enigmatic"
    };
    
    // Build all_names_ list
    for (const auto& [name, _] : scales_) {
        all_names_.push_back(name);
    }
}

std::vector<int> ScaleDictionary::getIntervals(const std::string& name) const {
    auto it = scales_.find(name);
    if (it != scales_.end()) {
        return it->second;
    }
    return {2, 2, 1, 2, 2, 2, 1};  // Default to Major
}

std::vector<std::string> ScaleDictionary::getAllScaleNames() const {
    return all_names_;
}

std::vector<std::string> ScaleDictionary::getScalesByCategory(const std::string& category) const {
    auto it = categories_.find(category);
    if (it != categories_.end()) {
        return it->second;
    }
    return {};
}

bool ScaleDictionary::hasScale(const std::string& name) const {
    return scales_.find(name) != scales_.end();
}

std::string ScaleDictionary::getRandomScaleName() const {
    if (all_names_.empty()) return "Major";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(all_names_.size()) - 1);
    return all_names_[dist(gen)];
}

} // namespace Music
