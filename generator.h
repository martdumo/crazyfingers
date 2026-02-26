#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <memory>
#include <random>
#include "fretboard.h"
#include "music_theory.h"

namespace Guitar {

// ============================================================================
// Constants - Biomechanically Organic Generation
// ============================================================================

constexpr int NUM_NOTES = 16;
constexpr int MAX_CONSECUTIVE_SAME_STRING = 3;
constexpr int MIN_CONSECUTIVE_SAME_STRING = 2;  // Allow 2-3 notes per string
constexpr int MAX_FRET_DELTA = 4;  // Absolute max (4 trastes = sorpresa)
constexpr int SLIDING_WINDOW_SIZE = 4;  // Check 4 consecutive notes
constexpr int MAX_WINDOW_RANGE = 5;  // Max range in any 4-note window

// Weight constants for organic movement
constexpr int WEIGHT_VERY_CLOSE = 70;  // 0-2 frets: very comfortable
constexpr int WEIGHT_MEDIUM = 20;       // 3 frets: moderate stretch
constexpr int WEIGHT_SURPRISE = 10;     // 4 frets: surprise note

// ============================================================================
// Random Engine with Weighted Distribution
// ============================================================================

class RandomEngine {
public:
    RandomEngine();
    
    [[nodiscard]] int generateInt(int min_val, int max_val);
    [[nodiscard]] bool generateBool();
    
    // Select index based on weights using discrete_distribution
    template<typename WeightType>
    [[nodiscard]] int selectWeighted(const std::vector<WeightType>& weights);
    
private:
    std::mt19937 engine_;
};

// ============================================================================
// Note Candidate with Weight
// ============================================================================

struct NoteCandidate {
    Note note;
    int weight;  // Probability weight based on distance
    int fret_distance;  // Absolute distance from previous note
};

// ============================================================================
// Note Generator - Organic Movement
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
    
    // Build list of valid candidates with weights
    [[nodiscard]] std::vector<NoteCandidate> buildCandidates(
        const Note& previous,
        bool must_change_string,
        const std::vector<std::unique_ptr<Note>>& previous_notes
    );
    
    // Calculate weight based on fret distance
    [[nodiscard]] int calculateWeight(int fret_distance) const;
    
    // Check if note satisfies sliding window constraint
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
