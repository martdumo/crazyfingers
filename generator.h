#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <memory>
#include <random>
#include "fretboard.h"
#include "music_theory.h"

namespace Guitar {

// ============================================================================
// Constants
// ============================================================================

constexpr int NUM_NOTES = 16;
constexpr int MAX_CONSECUTIVE_SAME_STRING = 3;
constexpr int MAX_FRET_DELTA = 3;
constexpr int SLIDING_WINDOW_SIZE = 3;
constexpr int MAX_WINDOW_RANGE = 5;

// ============================================================================
// Random Engine
// ============================================================================

class RandomEngine {
public:
    RandomEngine();
    
    [[nodiscard]] int generateInt(int min_val, int max_val);
    [[nodiscard]] bool generateBool();
    
private:
    std::mt19937 engine_;
};

// ============================================================================
// Note Generator
// ============================================================================

class NoteGenerator {
public:
    NoteGenerator(const FretboardValidator& validator);
    
    // Generate complete tablature (16 notes)
    [[nodiscard]] std::vector<std::unique_ptr<Note>> generateTablature();
    
private:
    [[nodiscard]] std::unique_ptr<Note> generateFirstNote();
    [[nodiscard]] std::unique_ptr<Note> generateNextNote(
        const Note& previous,
        int consecutive_same_string,
        bool must_change_string,
        const std::vector<std::unique_ptr<Note>>& previous_notes
    );
    
    [[nodiscard]] bool isValidForWindow(
        const Note& new_note,
        const std::vector<std::unique_ptr<Note>>& previous_notes
    ) const;
    
    const FretboardValidator& validator_;
    RandomEngine rng_;
    std::vector<Note> valid_notes_cache_;
};

// ============================================================================
// Tablature Generator (RAII Coordinator)
// ============================================================================

class TablatureGenerator {
public:
    TablatureGenerator();
    
    void generate();
    [[nodiscard]] const std::vector<std::unique_ptr<Note>>& getNotes() const noexcept;
    [[nodiscard]] const Music::ScaleManager& getScaleManager() const noexcept;
    
    // Non-copyable, movable
    TablatureGenerator(const TablatureGenerator&) = delete;
    TablatureGenerator& operator=(const TablatureGenerator&) = delete;
    TablatureGenerator(TablatureGenerator&&) = default;
    TablatureGenerator& operator=(TablatureGenerator&&) = default;
    ~TablatureGenerator() = default;
    
private:
    Music::ScaleManager scale_mgr_;
    std::unique_ptr<FretboardValidator> validator_;
    std::unique_ptr<NoteGenerator> note_gen_;
    std::vector<std::unique_ptr<Note>> notes_;
};

} // namespace Guitar

#endif // GENERATOR_H
