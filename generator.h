#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <memory>
#include "fretboard.h"
#include "music_theory.h"
#include "random_engine.h"

namespace Guitar {

// ============================================================================
// Constants - Position Box Heuristic
// ============================================================================

constexpr int NUM_NOTES = 16;
constexpr int MAX_CONSECUTIVE_SAME_STRING = 3;
constexpr int POSITION_BOX_RADIUS = 4;  // ±4 frets from anchor

// Weight constants for organic movement
constexpr int WEIGHT_CLOSE = 60;       // 0-2 frets: comfortable
constexpr int WEIGHT_MEDIUM = 30;      // 3 frets: moderate
constexpr int WEIGHT_FAR = 10;         // 4 frets: stretch

// ============================================================================
// Note Candidate with Weight
// ============================================================================

struct NoteCandidate {
    Note note;
    int weight;  // Probability weight based on distance
    int fret_distance;  // Absolute distance from previous note
};

// ============================================================================
// Position Box - Anchors the hand position for entire exercise
// ============================================================================

struct PositionBox {
    int anchor_fret;   // First note's fret - anchors the position
    int min_fret;      // Lower bound: max(0, anchor - 4)
    int max_fret;      // Upper bound: min(MAX_FRET, anchor + 4)

    [[nodiscard]] bool contains(int fret) const {
        return fret >= min_fret && fret <= max_fret;
    }

    void initialize(int first_fret);
};

// ============================================================================
// Note Generator - Position Box with Free String Skipping
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
        const PositionBox& box
    );

    // Calculate weight based on fret distance
    [[nodiscard]] int calculateWeight(int fret_distance) const;

    const FretboardValidator& validator_;
    RandomEngine rng_;
    std::vector<Note> valid_notes_cache_;
    PositionBox position_box_;  // Global position anchor for entire exercise
};

// ============================================================================
// Tablature Generator (RAII Coordinator)
// ============================================================================

class TablatureGenerator {
public:
    explicit TablatureGenerator(InstrumentType instrument);

    // Generate new tablature (random key/scale)
    void generate();

    // Regenerate with same key/scale (for re-roll)
    void regenerate();

    // Set specific key and scale (for advanced mode)
    void setKeyAndScale(Music::KeyIndex key, const std::string& scale_name);

    [[nodiscard]] const std::vector<std::unique_ptr<Note>>& getNotes() const noexcept;
    [[nodiscard]] const Music::ScaleManager& getScaleManager() const noexcept;
    [[nodiscard]] InstrumentType getInstrumentType() const noexcept;

    // Get current key index (for re-roll with same settings)
    [[nodiscard]] Music::KeyIndex getCurrentKeyIndex() const noexcept;
    [[nodiscard]] std::string getCurrentScaleName() const noexcept;

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
    bool use_random_settings_;  // Track if we're using random or fixed settings
};

} // namespace Guitar

#endif // GENERATOR_H
