#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <memory>
#include <random>
#include <optional>
#include "fretboard.h"
#include "music_theory.h"

namespace Guitar {

// ============================================================================
// Constants - Biomechanically Organic Generation
// ============================================================================

constexpr int NUM_NOTES = 16;
constexpr int MAX_CONSECUTIVE_SAME_STRING = 3;
constexpr int MIN_CONSECUTIVE_SAME_STRING = 2;  // Allow 2-3 notes per string
constexpr int MAX_FRET_DELTA = 4;  // Absolute max (4 frets = surprise)
constexpr int SLIDING_WINDOW_SIZE = 3;  // Check 3 consecutive notes (N, N-1, N-2)
constexpr int MAX_WINDOW_RANGE = 5;  // Max range in any 3-note window

// Weight constants for organic movement
constexpr int WEIGHT_VERY_CLOSE = 70;  // 0-2 frets: very comfortable
constexpr int WEIGHT_MEDIUM = 20;       // 3 frets: moderate stretch
constexpr int WEIGHT_SURPRISE = 10;     // 4 frets: surprise note

// Inertia constants
constexpr int INERTIA_BONUS_SMALL_MOVE = 40;   // Bonus for small move after big jump
constexpr int INERTIA_PENALTY_ZIGZAG = 50;     // Penalty for zigzag after big jump
constexpr int BIG_JUMP_THRESHOLD = 3;          // Fret delta considered a "big jump"
constexpr int SMALL_MOVE_DELTA = 1;            // What counts as a "small" move

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
    int weight;  // Probability weight based on distance and inertia
    int fret_distance;  // Absolute distance from previous note
};

// ============================================================================
// Movement History for Inertia Calculation
// ============================================================================

struct MovementHistory {
    std::optional<int> last_fret_delta;   // N-1 to N-2 movement
    std::optional<int> last_string_delta; // String change direction

    [[nodiscard]] bool wasBigJump() const {
        return last_fret_delta.has_value() && 
               std::abs(*last_fret_delta) >= BIG_JUMP_THRESHOLD;
    }

    [[nodiscard]] int lastDelta() const {
        return last_fret_delta.value_or(0);
    }
};

// ============================================================================
// Note Generator - Organic Movement with Inertia
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
        const std::vector<std::unique_ptr<Note>>& previous_notes,
        MovementHistory& history
    );

    // Build list of valid candidates with weights
    [[nodiscard]] std::vector<NoteCandidate> buildCandidates(
        const Note& previous,
        bool must_change_string,
        const std::vector<std::unique_ptr<Note>>& previous_notes,
        const MovementHistory& history
    );

    // Calculate base weight based on fret distance
    [[nodiscard]] int calculateBaseWeight(int fret_distance) const;

    // Apply inertia modifier to weight
    [[nodiscard]] int applyInertiaWeight(int base_weight, int fret_delta,
                                          const MovementHistory& history) const;

    // Check if note satisfies 3-note sliding window constraint
    [[nodiscard]] bool isValidForWindow(
        const Note& new_note,
        const std::vector<std::unique_ptr<Note>>& previous_notes
    ) const;

    // Calculate fret delta between two notes (can be on different strings)
    [[nodiscard]] int calculateFretDelta(const Note& from, const Note& to) const;

    const FretboardValidator& validator_;
    RandomEngine rng_;
    std::vector<Note> valid_notes_cache_;
};

// ============================================================================
// Tablature Generator (RAII Coordinator)
// ============================================================================

class TablatureGenerator {
public:
    explicit TablatureGenerator(InstrumentType instrument);

    void generate();
    [[nodiscard]] const std::vector<std::unique_ptr<Note>>& getNotes() const noexcept;
    [[nodiscard]] const Music::ScaleManager& getScaleManager() const noexcept;
    [[nodiscard]] InstrumentType getInstrumentType() const noexcept;

    // Non-copyable, movable
    TablatureGenerator(const TablatureGenerator&) = delete;
    TablatureGenerator& operator=(const TablatureGenerator&) = delete;
    TablatureGenerator(TablatureGenerator&&) = default;
    TablatureGenerator& operator=(TablatureGenerator&&) = default;
    ~TablatureGenerator() = default;

private:
    InstrumentType instrument_;
    Music::ScaleManager scale_mgr_;
    std::unique_ptr<FretboardValidator> validator_;
    std::unique_ptr<NoteGenerator> note_gen_;
    std::vector<std::unique_ptr<Note>> notes_;
};

} // namespace Guitar

#endif // GENERATOR_H
