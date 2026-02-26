#ifndef FRETBOARD_H
#define FRETBOARD_H

#include <array>
#include <cstdint>
#include "music_theory.h"

namespace Guitar {

// ============================================================================
// Constants
// ============================================================================

constexpr int NUM_STRINGS = 6;
constexpr int MIN_FRET = 0;
constexpr int MAX_FRET = 22;

// MIDI values for open strings (standard tuning)
// E2=40, A2=45, D3=50, G3=55, B3=59, E4=64
constexpr std::array<int, NUM_STRINGS> OPEN_STRING_MIDI = {
    64,  // 1st string (high e)
    59,  // 2nd string (B)
    55,  // 3rd string (G)
    50,  // 4th string (D)
    45,  // 5th string (A)
    40   // 6th string (low E)
};

// String labels for tablature
constexpr std::array<const char*, NUM_STRINGS> STRING_LABELS = {
    "e", "B", "G", "D", "A", "E"
};

// ============================================================================
// Data Structures
// ============================================================================

struct Fret {
    int value;
    
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return value >= MIN_FRET && value <= MAX_FRET;
    }
};

struct StringIndex {
    int value;  // 0 = high e, 5 = low E
    
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return value >= 0 && value < NUM_STRINGS;
    }
};

struct Note {
    StringIndex string_idx;
    Fret fret;
    
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return string_idx.isValid() && fret.isValid();
    }
    
    [[nodiscard]] constexpr int getMidiPitch() const noexcept {
        return OPEN_STRING_MIDI[string_idx.value] + fret.value;
    }
    
    [[nodiscard]] constexpr Music::PitchClass getPitchClass() const noexcept {
        return static_cast<Music::PitchClass>(getMidiPitch() % Music::SEMITONES_IN_OCTAVE);
    }
};

// ============================================================================
// Fretboard Validator
// ============================================================================

class FretboardValidator {
public:
    explicit FretboardValidator(const Music::ScaleManager& scale_mgr);
    
    // Check if a note is valid for current scale
    [[nodiscard]] bool isNoteInScale(const Note& note) const;
    
    // Check if two notes are anatomically possible to play consecutively
    [[nodiscard]] bool isAnatomicallyPossible(const Note& from, const Note& to,
                                               int max_fret_delta = 3) const;
    
    // Check sliding window constraint (3 consecutive notes)
    [[nodiscard]] bool validateSlidingWindow(const Note& note1, const Note& note2, 
                                              const Note& note3, int max_range = 5) const;
    
    // Get all valid notes on the fretboard for current scale
    [[nodiscard]] std::vector<Note> getAllValidNotes() const;

private:
    const Music::ScaleManager& scale_mgr_;
};

} // namespace Guitar

#endif // FRETBOARD_H
