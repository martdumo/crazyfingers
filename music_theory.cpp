#include "music_theory.h"
#include "scale_dictionary.h"
#include <random>
#include <algorithm>
#include <sstream>
#include <cctype>

namespace Music {

// ============================================================================
// Utility Functions Implementation
// ============================================================================

std::string pitchClassToName(PitchClass pc) {
    if (pc < NUM_KEYS) {
        return std::string(NOTE_NAMES[pc]);
    }
    return "?";
}

std::string computeScaleNotes(KeyIndex root, const std::vector<int>& intervals) {
    std::ostringstream oss;

    PitchClass current = root;
    oss << NOTE_NAMES[current];

    for (int interval : intervals) {
        current = static_cast<PitchClass>((current + interval) % SEMITONES_IN_OCTAVE);
        oss << " " << NOTE_NAMES[current];
    }

    return oss.str();
}

int parseKeyName(const std::string& name) {
    if (name.empty()) return -1;
    
    // Convert to uppercase for case-insensitive comparison
    std::string upperName;
    upperName.reserve(name.size());
    for (char c : name) {
        upperName.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }
    
    // Search through KEY_NAMES
    for (int i = 0; i < NUM_KEYS; ++i) {
        if (upperName == KEY_NAMES[i]) {
            return i;
        }
    }
    
    return -1;  // Invalid key name
}

std::vector<std::string> getScalesWithIds() {
    return ScaleDictionary::getInstance().getAllScaleNames();
}

// ============================================================================
// ScaleManager Implementation
// ============================================================================

ScaleManager::ScaleManager() 
    : current_key_{0}
    , current_scale_name_{"Major"}
    , current_intervals_{2, 2, 1, 2, 2, 2, 1}
    , valid_pitch_classes_{}
    , scale_notes_{} {
    selectRandomKeyAndScale();
}

void ScaleManager::selectRandomKeyAndScale() {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Random key (0-11)
    std::uniform_int_distribution<> key_dist(0, NUM_KEYS - 1);
    current_key_ = static_cast<KeyIndex>(key_dist(gen));

    // Get random scale from dictionary
    const auto& dict = ScaleDictionary::getInstance();
    current_scale_name_ = dict.getRandomScaleName();
    current_intervals_ = dict.getIntervals(current_scale_name_);

    // Compute valid pitch classes and note names
    computeValidPitchClasses();
    computeScaleNotes();
}

void ScaleManager::setKeyAndScale(KeyIndex key, const std::string& scale_name) {
    current_key_ = key;
    current_scale_name_ = scale_name;
    
    // Get intervals for the selected scale
    const auto& dict = ScaleDictionary::getInstance();
    current_intervals_ = dict.getIntervals(scale_name);
    
    // Compute valid pitch classes and note names
    computeValidPitchClasses();
    computeScaleNotes();
}

void ScaleManager::computeValidPitchClasses() {
    valid_pitch_classes_.clear();
    
    PitchClass current = current_key_;
    valid_pitch_classes_.push_back(current);
    
    for (int interval : current_intervals_) {
        current = static_cast<PitchClass>((current + interval) % SEMITONES_IN_OCTAVE);
        valid_pitch_classes_.push_back(current);
    }
    
    // Sort and remove duplicates
    std::sort(valid_pitch_classes_.begin(), valid_pitch_classes_.end());
    valid_pitch_classes_.erase(
        std::unique(valid_pitch_classes_.begin(), valid_pitch_classes_.end()),
        valid_pitch_classes_.end()
    );
}

void ScaleManager::computeScaleNotes() {
    scale_notes_ = Music::computeScaleNotes(current_key_, current_intervals_);
}

std::string ScaleManager::getCurrentKeyName() const {
    return std::string(KEY_NAMES[current_key_]);
}

KeyIndex ScaleManager::getCurrentKeyIndex() const {
    return current_key_;
}

std::string ScaleManager::getCurrentScaleName() const {
    return current_scale_name_;
}

std::string ScaleManager::getFullDescription() const {
    return getCurrentKeyName() + " " + getCurrentScaleName();
}

std::string ScaleManager::getScaleNotes() const {
    return scale_notes_;
}

bool ScaleManager::isPitchInScale(PitchClass pitch) const {
    return std::find(valid_pitch_classes_.begin(), 
                     valid_pitch_classes_.end(), 
                     pitch) != valid_pitch_classes_.end();
}

bool ScaleManager::isMidiPitchValid(int midi_pitch) const {
    PitchClass pc = static_cast<PitchClass>(midi_pitch % SEMITONES_IN_OCTAVE);
    return isPitchInScale(pc);
}

const std::vector<PitchClass>& ScaleManager::getValidPitchClasses() const {
    return valid_pitch_classes_;
}

} // namespace Music
