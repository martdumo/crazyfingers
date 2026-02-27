#ifndef FRETBOARD_H
#define FRETBOARD_H

#include <array>
#include <cstdint>
#include <string>
#include "music_theory.h"

namespace Guitar {

// ============================================================================
// Instrument Type Enumeration
// ============================================================================

enum class InstrumentType {
    Guitar,  // 6 strings, standard tuning
    Bass     // 4 strings, standard tuning
};

// ============================================================================
// Constants - Guitar (6 strings)
// ============================================================================

constexpr int GUITAR_NUM_STRINGS = 6;
constexpr int BASS_NUM_STRINGS = 4;
constexpr int MIN_FRET = 0;
constexpr int MAX_FRET = 22;

// MIDI values for open strings
// Guitar: E2=40, A2=45, D3=50, G3=55, B3=59, E4=64
// Bass:   E1=28, A1=33, D2=38, G2=43
constexpr std::array<int, GUITAR_NUM_STRINGS> GUITAR_OPEN_STRING_MIDI = {
    64,  // 1st string (high e)
    59,  // 2nd string (B)
    55,  // 3rd string (G)
    50,  // 4th string (D)
    45,  // 5th string (A)
    40   // 6th string (low E)
};

constexpr std::array<int, BASS_NUM_STRINGS> BASS_OPEN_STRING_MIDI = {
    43,  // 1st string (G)
    38,  // 2nd string (D)
    33,  // 3rd string (A)
    28   // 4th string (low E)
};

// String labels for tablature
constexpr std::array<const char*, GUITAR_NUM_STRINGS> GUITAR_STRING_LABELS = {
    "e", "B", "G", "D", "A", "E"
};

constexpr std::array<const char*, BASS_NUM_STRINGS> BASS_STRING_LABELS = {
    "G", "D", "A", "E"
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
    int value;  // 0 = highest pitch string, N-1 = lowest
    int num_strings;  // Total strings for this instrument

    [[nodiscard]] constexpr bool isValid() const noexcept {
        return value >= 0 && value < num_strings;
    }
};

struct Note {
    StringIndex string_idx;
    Fret fret;

    [[nodiscard]] constexpr bool isValid() const noexcept {
        return string_idx.isValid() && fret.isValid();
    }

    [[nodiscard]] int getMidiPitch() const noexcept {
        if (string_idx.num_strings == GUITAR_NUM_STRINGS) {
            return GUITAR_OPEN_STRING_MIDI[string_idx.value] + fret.value;
        } else {
            return BASS_OPEN_STRING_MIDI[string_idx.value] + fret.value;
        }
    }

    [[nodiscard]] Music::PitchClass getPitchClass() const noexcept {
        return static_cast<Music::PitchClass>(getMidiPitch() % Music::SEMITONES_IN_OCTAVE);
    }
};

// ============================================================================
// Instrument Configuration
// ============================================================================

struct InstrumentConfig {
    InstrumentType type;
    int num_strings;
    std::string name;

    [[nodiscard]] const std::array<int, 6>& getOpenStringMidi() const {
        static const std::array<int, 6> guitar_midi = GUITAR_OPEN_STRING_MIDI;
        static const std::array<int, 6> bass_midi = {43, 38, 33, 28, 0, 0};
        return (type == InstrumentType::Guitar) ? guitar_midi : bass_midi;
    }

    [[nodiscard]] const std::array<const char*, 6>& getStringLabels() const {
        static const std::array<const char*, 6> guitar_labels = GUITAR_STRING_LABELS;
        static const std::array<const char*, 6> bass_labels = {"G", "D", "A", "E", "", ""};
        return (type == InstrumentType::Guitar) ? guitar_labels : bass_labels;
    }
};

[[nodiscard]] constexpr InstrumentConfig getInstrumentConfig(InstrumentType type) {
    if (type == InstrumentType::Bass) {
        return {InstrumentType::Bass, BASS_NUM_STRINGS, "Bass"};
    }
    return {InstrumentType::Guitar, GUITAR_NUM_STRINGS, "Guitar"};
}

// ============================================================================
// Fretboard Validator
// ============================================================================

class FretboardValidator {
public:
    explicit FretboardValidator(const Music::ScaleManager& scale_mgr, InstrumentType instrument);

    // Get instrument configuration
    [[nodiscard]] const InstrumentConfig& getInstrument() const noexcept { return instrument_; }

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
    InstrumentConfig instrument_;
};

} // namespace Guitar

#endif // FRETBOARD_H
