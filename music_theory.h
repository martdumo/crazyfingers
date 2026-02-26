#ifndef MUSIC_THEORY_H
#define MUSIC_THEORY_H

#include <array>
#include <string>
#include <vector>
#include <cstdint>

namespace Music {

// ============================================================================
// Constants
// ============================================================================

constexpr int SEMITONES_IN_OCTAVE = 12;
constexpr int NUM_KEYS = 12;

// ============================================================================
// Types
// ============================================================================

using KeyIndex = uint8_t;
using Interval = uint8_t;
using PitchClass = uint8_t;  // 0-11 (C, C#, D, ..., B)

// ============================================================================
// Key Names (0=C, 1=C#, 2=D, ..., 11=B)
// ============================================================================

constexpr std::array<const char*, NUM_KEYS> KEY_NAMES = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

// ============================================================================
// Scale Definitions (intervals in semitones)
// ============================================================================

struct ScaleDefinition {
    const char* name;
    std::vector<Interval> intervals;
};

// ============================================================================
// Scale Manager Class
// ============================================================================

class ScaleManager {
public:
    ScaleManager();
    
    // Select random key and scale
    void selectRandomKeyAndScale();
    
    // Get current key name (e.g., "F#")
    [[nodiscard]] std::string getCurrentKeyName() const;
    
    // Get current scale name (e.g., "Pentatonic Minor")
    [[nodiscard]] std::string getCurrentScaleName() const;
    
    // Get full description (e.g., "F# - Pentatonic Minor")
    [[nodiscard]] std::string getFullDescription() const;
    
    // Check if a pitch class (0-11) belongs to current scale
    [[nodiscard]] bool isPitchInScale(PitchClass pitch) const;
    
    // Check if a MIDI pitch is valid for current key/scale
    [[nodiscard]] bool isMidiPitchValid(int midi_pitch) const;
    
    // Get all valid pitch classes for current scale
    [[nodiscard]] const std::vector<PitchClass>& getValidPitchClasses() const;

private:
    void computeValidPitchClasses();
    void initializeScales();
    
    KeyIndex current_key_;
    ScaleDefinition current_scale_;
    std::vector<PitchClass> valid_pitch_classes_;
    std::vector<ScaleDefinition> all_scales_;
};

} // namespace Music

#endif // MUSIC_THEORY_H
