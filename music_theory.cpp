#include "music_theory.h"
#include <random>
#include <algorithm>

namespace Music {

// ============================================================================
// ScaleManager Implementation
// ============================================================================

ScaleManager::ScaleManager() 
    : current_key_{0}
    , current_scale_{"Major", {}}
    , valid_pitch_classes_{}
    , all_scales_{} {
    initializeScales();
    selectRandomKeyAndScale();
}

void ScaleManager::initializeScales() {
    all_scales_ = {
        {"Major", {2, 2, 1, 2, 2, 2, 1}},
        {"Natural Minor", {2, 1, 2, 2, 1, 2, 2}},
        {"Pentatonic Major", {2, 2, 3, 2, 3}},
        {"Pentatonic Minor", {3, 2, 2, 3, 2}},
        {"Blues", {3, 2, 1, 1, 3, 2}},
        {"Dorian", {2, 1, 2, 2, 2, 1, 2}},
        {"Phrygian", {1, 2, 2, 2, 1, 2, 2}},
        {"Lydian", {2, 2, 2, 1, 2, 2, 1}},
        {"Mixolydian", {2, 2, 1, 2, 2, 1, 2}},
        {"Locrian", {1, 2, 2, 1, 2, 2, 2}},
        {"Dominant Bebop", {2, 2, 1, 2, 2, 1, 1, 2}},
        {"Major Bebop", {2, 2, 1, 2, 1, 1, 2, 2}},
        {"Harmonic Minor", {2, 1, 2, 2, 1, 3, 1}},
        {"Melodic Minor", {2, 1, 2, 2, 2, 2, 1}},
        {"Arabic (Double Harmonic)", {1, 3, 1, 2, 1, 3, 1}},
        {"Hirajoshi", {3, 1, 4, 1, 3}},
        {"Hungarian Minor", {2, 1, 3, 1, 1, 3, 1}}
    };
}

void ScaleManager::selectRandomKeyAndScale() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Random key (0-11)
    std::uniform_int_distribution<> key_dist(0, NUM_KEYS - 1);
    current_key_ = static_cast<KeyIndex>(key_dist(gen));
    
    // Random scale
    std::uniform_int_distribution<> scale_dist(0, static_cast<int>(all_scales_.size()) - 1);
    current_scale_ = all_scales_[scale_dist(gen)];
    
    // Compute valid pitch classes
    computeValidPitchClasses();
}

void ScaleManager::computeValidPitchClasses() {
    valid_pitch_classes_.clear();
    
    PitchClass current = current_key_;
    valid_pitch_classes_.push_back(current);
    
    for (Interval interval : current_scale_.intervals) {
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

std::string ScaleManager::getCurrentKeyName() const {
    return std::string(KEY_NAMES[current_key_]);
}

std::string ScaleManager::getCurrentScaleName() const {
    return std::string(current_scale_.name);
}

std::string ScaleManager::getFullDescription() const {
    return getCurrentKeyName() + " - " + getCurrentScaleName();
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
