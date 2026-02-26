// Crazy Fingers - Guitar Tablature Generator
// C++20 implementation following C++ Core Guidelines

#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <array>
#include <algorithm>
#include <cstdlib>

namespace GuitarTab {

// ============================================================================
// Constants (Compile-time evaluation) [CG: F.4, P.10]
// ============================================================================

constexpr int NUM_STRINGS = 6;
constexpr int MIN_FRET = 0;
constexpr int MAX_FRET = 22;
constexpr int NUM_NOTES = 16;
constexpr int MAX_FRET_DELTA = 5;
constexpr int MAX_CONSECUTIVE_SAME_STRING = 2;
constexpr int MIN_STRINGS_USED = 3;

// Standard tuning: e, B, G, D, A, E (high to low)
constexpr std::array<const char*, NUM_STRINGS> STRING_LABELS = {
    "e", "B", "G", "D", "A", "E"
};

// ============================================================================
// Data Structures [CG: C.10]
// ============================================================================

struct Fret {
    int value;  // 0-22 (0 = open string)
    
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return value >= MIN_FRET && value <= MAX_FRET;
    }
};

struct StringIndex {
    int value;  // 0-5 (0 = high e, 5 = low E)
    
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
};

// ============================================================================
// Random Number Generator [CG: R.11 - no naked new/delete]
// ============================================================================

class RandomEngine {
public:
    RandomEngine() 
        : engine_{std::random_device{}()}
        , string_delta_dist_{-1, 1}
        , fret_delta_dist_{-MAX_FRET_DELTA, MAX_FRET_DELTA}
        , force_direction_dist_{0, 1}
        , string_dist_{1, 4}
        , fret_dist_{3, 15} {}
    
    [[nodiscard]] int generateStringDelta() {
        return string_delta_dist_(engine_);
    }
    
    [[nodiscard]] int generateFretDelta() {
        return fret_delta_dist_(engine_);
    }
    
    [[nodiscard]] bool shouldMoveUp() {
        return force_direction_dist_(engine_) == 0;
    }
    
    [[nodiscard]] int generateInitialString() {
        return string_dist_(engine_);
    }
    
    [[nodiscard]] int generateInitialFret() {
        return fret_dist_(engine_);
    }
    
private:
    mutable std::mt19937 engine_;
    mutable std::uniform_int_distribution<int> string_delta_dist_;
    mutable std::uniform_int_distribution<int> fret_delta_dist_;
    mutable std::uniform_int_distribution<int> force_direction_dist_;
    mutable std::uniform_int_distribution<int> string_dist_;
    mutable std::uniform_int_distribution<int> fret_dist_;
};

// ============================================================================
// Note Validation [CG: F.2 - single purpose]
// ============================================================================

[[nodiscard]] constexpr bool isAnatomicallyPossible(const Note& from, const Note& to) noexcept {
    const int string_distance = std::abs(to.string_idx.value - from.string_idx.value);
    const int fret_distance = std::abs(to.fret.value - from.fret.value);
    
    // Rule: Only same or adjacent strings allowed
    if (string_distance > 1) {
        return false;
    }
    
    // Rule: Max 5 frets distance
    if (fret_distance > MAX_FRET_DELTA) {
        return false;
    }
    
    // Rule: Must be a different note (no repeats for flexibility exercise)
    if (string_distance == 0 && fret_distance == 0) {
        return false;
    }
    
    return true;
}

// ============================================================================
// Note Generator [CG: F.3 - logical operation]
// ============================================================================

class NoteGenerator {
public:
    NoteGenerator() : random_engine_{} {}
    
    [[nodiscard]] std::unique_ptr<Note> generateFirstNote() {
        auto note = std::make_unique<Note>();
        
        // Start on a random string (prefer middle strings for flexibility)
        note->string_idx.value = random_engine_.generateInitialString();
        
        // Start on a random fret (prefer middle frets)
        note->fret.value = random_engine_.generateInitialFret();
        
        return note;
    }
    
    [[nodiscard]] std::unique_ptr<Note> generateNextNote(
        const Note& previous,
        int consecutive_same_string,
        bool must_change_string
    ) {
        auto note = std::make_unique<Note>();
        int attempts = 0;
        const int max_attempts = 50;
        
        do {
            // Determine string movement
            if (must_change_string || consecutive_same_string >= MAX_CONSECUTIVE_SAME_STRING) {
                // Force string change (adjacent only)
                const int direction = random_engine_.shouldMoveUp() ? -1 : 1;
                note->string_idx.value = previous.string_idx.value + direction;
                
                // Clamp to valid range
                if (note->string_idx.value < 0) {
                    note->string_idx.value = 0;
                } else if (note->string_idx.value >= NUM_STRINGS) {
                    note->string_idx.value = NUM_STRINGS - 1;
                }
                
                // If clamping put us on same string, force the other direction
                if (note->string_idx.value == previous.string_idx.value) {
                    if (previous.string_idx.value == 0) {
                        note->string_idx.value = 1;
                    } else if (previous.string_idx.value == NUM_STRINGS - 1) {
                        note->string_idx.value = NUM_STRINGS - 2;
                    }
                }
            } else {
                // Can stay or move to adjacent
                const int delta = random_engine_.generateStringDelta();
                note->string_idx.value = previous.string_idx.value + delta;
                
                // Clamp to valid range
                if (note->string_idx.value < 0) {
                    note->string_idx.value = 0;
                } else if (note->string_idx.value >= NUM_STRINGS) {
                    note->string_idx.value = NUM_STRINGS - 1;
                }
            }
            
            // Determine fret movement
            const int fret_delta = random_engine_.generateFretDelta();
            note->fret.value = previous.fret.value + fret_delta;
            
            // Clamp to valid fret range
            if (note->fret.value < MIN_FRET) {
                note->fret.value = MIN_FRET;
            } else if (note->fret.value > MAX_FRET) {
                note->fret.value = MAX_FRET;
            }
            
            attempts++;
            
        } while (!isAnatomicallyPossible(previous, *note) && attempts < max_attempts);
        
        // If we couldn't find a valid note, create a fallback
        if (!isAnatomicallyPossible(previous, *note)) {
            note->string_idx.value = (previous.string_idx.value + 1) % NUM_STRINGS;
            note->fret.value = std::clamp(previous.fret.value + 2, MIN_FRET, MAX_FRET);
        }
        
        return note;
    }
    
private:
    RandomEngine random_engine_;
};

// ============================================================================
// Tablature Validator [CG: F.2 - single purpose]
// ============================================================================

[[nodiscard]] bool validateTablature(const std::vector<std::unique_ptr<Note>>& notes) {
    if (notes.size() != NUM_NOTES) {
        return false;
    }
    
    // Check all notes are valid
    for (const auto& note : notes) {
        if (note == nullptr || !note->isValid()) {
            return false;
        }
    }
    
    // Check minimum strings used
    std::array<bool, NUM_STRINGS> strings_used{};
    strings_used.fill(false);
    
    for (const auto& note : notes) {
        strings_used[note->string_idx.value] = true;
    }
    
    int unique_strings = 0;
    for (const bool used : strings_used) {
        if (used) unique_strings++;
    }
    
    if (unique_strings < MIN_STRINGS_USED) {
        return false;
    }
    
    // Check no more than 2 consecutive on same string
    int consecutive = 1;
    for (size_t i = 1; i < notes.size(); ++i) {
        if (notes[i]->string_idx.value == notes[i-1]->string_idx.value) {
            consecutive++;
            if (consecutive > MAX_CONSECUTIVE_SAME_STRING) {
                return false;
            }
        } else {
            consecutive = 1;
        }
    }
    
    return true;
}

// ============================================================================
// Tablature Generator (RAII Coordinator) [CG: R.1, C.149]
// ============================================================================

class TablatureGenerator {
public:
    TablatureGenerator() 
        : notes_{}
        , generator_{} {}
    
    void generate() {
        notes_.clear();
        notes_.reserve(NUM_NOTES);
        
        // Generate first note
        notes_.push_back(generator_.generateFirstNote());
        
        // Generate remaining notes
        int consecutive_same_string = 0;
        
        for (int i = 1; i < NUM_NOTES; ++i) {
            const Note& previous = *notes_[i - 1];
            const bool must_change = (consecutive_same_string >= MAX_CONSECUTIVE_SAME_STRING);
            
            auto next_note = generator_.generateNextNote(previous, consecutive_same_string, must_change);
            
            // Track consecutive same string
            if (next_note->string_idx.value == previous.string_idx.value) {
                consecutive_same_string++;
            } else {
                consecutive_same_string = 0;
            }
            
            notes_.push_back(std::move(next_note));
        }
        
        // Validate and regenerate if necessary
        if (!validateTablature(notes_)) {
            // Simple retry (in production, would use more sophisticated approach)
            generate();
        }
    }
    
    [[nodiscard]] const std::vector<std::unique_ptr<Note>>& getNotes() const noexcept {
        return notes_;
    }
    
    // [CG: C.61] - Non-copyable, movable
    TablatureGenerator(const TablatureGenerator&) = delete;
    TablatureGenerator& operator=(const TablatureGenerator&) = delete;
    TablatureGenerator(TablatureGenerator&&) = default;
    TablatureGenerator& operator=(TablatureGenerator&&) = default;
    
    ~TablatureGenerator() = default;  // RAII: vector cleans up automatically
    
private:
    std::vector<std::unique_ptr<Note>> notes_;  // [CG: R.11, C.149]
    NoteGenerator generator_;
};

// ============================================================================
// Tablature Formatter (Output) [CG: SF.20 - namespace structure]
// ============================================================================

namespace Formatter {

constexpr int NOTE_WIDTH = 4;  // "---X" format

[[nodiscard]] std::string formatNotePosition(const Note* note, int current_string) {
    if (note == nullptr) {
        return "---";
    }
    
    if (note->string_idx.value == current_string) {
        const std::string fret_str = std::to_string(note->fret.value);
        return fret_str;
    }
    
    return "---";
}

void printTablature(const std::vector<std::unique_ptr<Note>>& notes) {
    // Print each string from high e (index 0) to low E (index 5)
    for (int string_idx = 0; string_idx < NUM_STRINGS; ++string_idx) {
        std::cout << STRING_LABELS[string_idx] << "|";
        
        for (const auto& note : notes) {
            const std::string position = formatNotePosition(note.get(), string_idx);
            
            // Pad to NOTE_WIDTH
            std::cout << "-";
            if (position.length() == 1) {
                std::cout << position << "--";
            } else if (position.length() == 2) {
                std::cout << position << "-";
            } else {
                std::cout << position;
            }
        }
        
        std::cout << "|" << std::endl;
    }
}

} // namespace Formatter

} // namespace GuitarTab

// ============================================================================
// Entry Point [CG: R.1 - RAII cleanup]
// ============================================================================

int main() {
    // Create generator (RAII - will clean up on scope exit)
    GuitarTab::TablatureGenerator generator;
    
    // Generate tablature
    generator.generate();
    
    // Print directly to console
    GuitarTab::Formatter::printTablature(generator.getNotes());
    
    return 0;  // Immediate exit, no user input
}
