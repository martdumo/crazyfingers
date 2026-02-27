#include "generator.h"
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>

namespace Guitar {

// ============================================================================
// RandomEngine Implementation
// ============================================================================

RandomEngine::RandomEngine() : engine_{std::random_device{}()} {}

int RandomEngine::generateInt(int min_val, int max_val) {
    std::uniform_int_distribution<> dist(min_val, max_val);
    return dist(engine_);
}

bool RandomEngine::generateBool() {
    std::uniform_int_distribution<> dist(0, 1);
    return dist(engine_) == 0;
}

template<typename WeightType>
int RandomEngine::selectWeighted(const std::vector<WeightType>& weights) {
    if (weights.empty()) return -1;
    if (weights.size() == 1) return 0;

    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    return dist(engine_);
}

// Explicit template instantiation
template int RandomEngine::selectWeighted<int>(const std::vector<int>&);

// ============================================================================
// NoteGenerator Implementation
// ============================================================================

NoteGenerator::NoteGenerator(const FretboardValidator& validator)
    : validator_{validator}
    , rng_{}
    , valid_notes_cache_{validator.getAllValidNotes()} {}

std::vector<std::unique_ptr<Note>> NoteGenerator::generateTablature() {
    std::vector<std::unique_ptr<Note>> notes;
    notes.reserve(NUM_NOTES);

    // Generate first note
    notes.push_back(generateFirstNote());

    // Generate remaining notes with movement history
    int consecutive_same_string = 0;
    MovementHistory history;

    for (int i = 1; i < NUM_NOTES; ++i) {
        const Note& previous = *notes[i - 1];

        // Determine if we MUST change string
        const bool must_change = (consecutive_same_string >= MAX_CONSECUTIVE_SAME_STRING);

        // Also force change if we've been on same string for MIN consecutive
        // and random chance says to move (promotes variety)
        const bool should_force_change = must_change ||
            (consecutive_same_string >= MIN_CONSECUTIVE_SAME_STRING &&
             rng_.generateInt(0, 2) == 0);  // 33% chance to move after 2 notes

        auto next_note = generateNextNote(previous, consecutive_same_string,
                                          should_force_change, notes, history);

        // Update movement history for next iteration
        if (notes.size() >= 2) {
            const Note& prev_prev = *notes[notes.size() - 2];
            history.last_fret_delta = calculateFretDelta(prev_prev, previous);
            history.last_string_delta = next_note->string_idx.value - previous.string_idx.value;
        }

        // Track consecutive same string
        if (next_note->string_idx.value == previous.string_idx.value) {
            consecutive_same_string++;
        } else {
            consecutive_same_string = 0;
        }

        notes.push_back(std::move(next_note));
    }

    return notes;
}

std::unique_ptr<Note> NoteGenerator::generateFirstNote() {
    auto note = std::make_unique<Note>();

    // Prefer middle strings and frets for ergonomic starting position
    note->string_idx.value = rng_.generateInt(1, validator_.getInstrument().num_strings - 2);
    note->fret.value = rng_.generateInt(5, 12);
    note->string_idx.num_strings = validator_.getInstrument().num_strings;

    // Ensure note is in scale
    int attempts = 0;
    while (!validator_.isNoteInScale(*note) && attempts < 50) {
        note->string_idx.value = rng_.generateInt(1, validator_.getInstrument().num_strings - 2);
        note->fret.value = rng_.generateInt(5, 12);
        attempts++;
    }

    // Fallback: find any valid note
    if (!validator_.isNoteInScale(*note) && !valid_notes_cache_.empty()) {
        *note = valid_notes_cache_[rng_.generateInt(0, static_cast<int>(valid_notes_cache_.size()) - 1)];
    }

    return note;
}

std::unique_ptr<Note> NoteGenerator::generateNextNote(
    const Note& previous,
    int /* consecutive_same_string */,
    bool must_change_string,
    const std::vector<std::unique_ptr<Note>>& previous_notes,
    MovementHistory& history
) {
    // Build list of valid candidates with weights (including inertia)
    auto candidates = buildCandidates(previous, must_change_string, previous_notes, history);

    // Emergency fallback if no candidates
    if (candidates.empty()) {
        auto note = std::make_unique<Note>();
        note->string_idx.value = (previous.string_idx.value < validator_.getInstrument().num_strings / 2) ?
                                 previous.string_idx.value + 1 : previous.string_idx.value - 1;
        note->fret.value = previous.fret.value;
        note->string_idx.num_strings = validator_.getInstrument().num_strings;

        if (note->string_idx.value < 0) note->string_idx.value = 0;
        if (note->string_idx.value >= validator_.getInstrument().num_strings) 
            note->string_idx.value = validator_.getInstrument().num_strings - 1;

        return note;
    }

    // Extract weights
    std::vector<int> weights;
    weights.reserve(candidates.size());
    for (const auto& cand : candidates) {
        weights.push_back(cand.weight);
    }

    // Select candidate based on weights
    int selected_idx = rng_.selectWeighted(weights);

    auto note = std::make_unique<Note>();
    *note = candidates[selected_idx].note;

    return note;
}

std::vector<NoteCandidate> NoteGenerator::buildCandidates(
    const Note& previous,
    bool must_change_string,
    const std::vector<std::unique_ptr<Note>>& previous_notes,
    const MovementHistory& history
) {
    std::vector<NoteCandidate> candidates;
    candidates.reserve(27);  // Max: 3 strings x 9 frets (+-4)

    // Determine which strings to consider
    std::vector<int> strings_to_check;

    if (must_change_string) {
        // Must change to adjacent string
        if (previous.string_idx.value > 0) {
            strings_to_check.push_back(previous.string_idx.value - 1);
        }
        if (previous.string_idx.value < validator_.getInstrument().num_strings - 1) {
            strings_to_check.push_back(previous.string_idx.value + 1);
        }
    } else {
        // Can stay on same string or move to adjacent
        strings_to_check.push_back(previous.string_idx.value);  // Same string
        if (previous.string_idx.value > 0) {
            strings_to_check.push_back(previous.string_idx.value - 1);
        }
        if (previous.string_idx.value < validator_.getInstrument().num_strings - 1) {
            strings_to_check.push_back(previous.string_idx.value + 1);
        }
    }

    // For each string, find valid fret candidates
    for (int str : strings_to_check) {
        // Check frets from -MAX_FRET_DELTA to +MAX_FRET_DELTA
        for (int delta = -MAX_FRET_DELTA; delta <= MAX_FRET_DELTA; ++delta) {
            int test_fret = previous.fret.value + delta;

            // Validate fret range
            if (test_fret < MIN_FRET || test_fret > MAX_FRET) continue;

            // Skip if same note as previous (must be different)
            if (str == previous.string_idx.value && delta == 0) continue;

            Note candidate_note{{str, validator_.getInstrument().num_strings}, {test_fret}};

            // Check if note is in scale
            if (!validator_.isNoteInScale(candidate_note)) continue;

            // Check anatomical possibility (max 4 frets)
            int fret_distance = std::abs(delta);
            if (fret_distance > MAX_FRET_DELTA) continue;

            // Check 3-note sliding window constraint (N, N-1, N-2 must fit in 5 frets)
            if (!isValidForWindow(candidate_note, previous_notes)) continue;

            // Calculate base weight based on fret distance
            int weight = calculateBaseWeight(fret_distance);

            // Apply inertia modifier (rewards settling after big jumps, penalizes zigzag)
            weight = applyInertiaWeight(weight, delta, history);

            // Add bonus weight for staying on same string (promotes fluency)
            if (str == previous.string_idx.value) {
                weight = static_cast<int>(weight * 1.3);  // 30% bonus
            }

            candidates.push_back({candidate_note, weight, fret_distance});
        }
    }

    return candidates;
}

int NoteGenerator::calculateBaseWeight(int fret_distance) const {
    // Weight system based on comfort level
    switch (fret_distance) {
        case 0:
            // Same fret, different string (very comfortable)
            return WEIGHT_VERY_CLOSE;
        case 1:
            // 1 fret distance (very comfortable - adjacent scale degrees)
            return WEIGHT_VERY_CLOSE;
        case 2:
            // 2 frets distance (comfortable - common interval)
            return WEIGHT_VERY_CLOSE;
        case 3:
            // 3 frets distance (moderate stretch)
            return WEIGHT_MEDIUM;
        case 4:
            // 4 frets distance (surprise note - use sparingly)
            return WEIGHT_SURPRISE;
        default:
            // Should not reach here, but return 0 for safety
            return 0;
    }
}

int NoteGenerator::applyInertiaWeight(int base_weight, int fret_delta,
                                       const MovementHistory& history) const {
    // If last movement was NOT a big jump, no inertia adjustment needed
    if (!history.wasBigJump()) {
        return base_weight;
    }

    const int last_delta = history.lastDelta();

    // INERTIA RULE 1: After a big jump, reward small moves (0, +1, -1)
    // This represents "settling" the hand after a stretch
    if (std::abs(fret_delta) <= SMALL_MOVE_DELTA) {
        return base_weight + INERTIA_BONUS_SMALL_MOVE;
    }

    // INERTIA RULE 2: Penalize zigzag (big jump in opposite direction)
    // If last was +3 or more, penalize -3 or more (and vice versa)
    if ((last_delta > 0 && fret_delta < -BIG_JUMP_THRESHOLD) ||
        (last_delta < 0 && fret_delta > BIG_JUMP_THRESHOLD)) {
        return std::max(0, base_weight - INERTIA_PENALTY_ZIGZAG);
    }

    // INERTIA RULE 3: Continuing in same direction is okay (slight penalty for another big jump)
    if ((last_delta > 0 && fret_delta > 0) || (last_delta < 0 && fret_delta < 0)) {
        if (std::abs(fret_delta) >= BIG_JUMP_THRESHOLD) {
            return std::max(0, base_weight - 10);  // Small penalty for consecutive big jumps
        }
    }

    return base_weight;
}

bool NoteGenerator::isValidForWindow(
    const Note& new_note,
    const std::vector<std::unique_ptr<Note>>& previous_notes
) const {
    // Need at least 2 previous notes to form a 3-note window
    if (previous_notes.size() < SLIDING_WINDOW_SIZE - 1) {
        return true;
    }

    // Check the last 2 notes + new note (3-note window: N, N-1, N-2)
    size_t start_idx = previous_notes.size() - (SLIDING_WINDOW_SIZE - 1);

    int min_fret = new_note.fret.value;
    int max_fret = new_note.fret.value;

    for (size_t i = start_idx; i < previous_notes.size(); ++i) {
        int f = previous_notes[i]->fret.value;
        if (f < min_fret) min_fret = f;
        if (f > max_fret) max_fret = f;
    }

    // STRICT RULE: Hand span in any 3-note group must not exceed 5 frets
    return (max_fret - min_fret) <= MAX_WINDOW_RANGE;
}

int NoteGenerator::calculateFretDelta(const Note& from, const Note& to) const {
    // Simple fret difference (positive = up the neck, negative = down)
    return to.fret.value - from.fret.value;
}

// ============================================================================
// TablatureGenerator Implementation
// ============================================================================

TablatureGenerator::TablatureGenerator(InstrumentType instrument)
    : instrument_{instrument}
    , scale_mgr_{}
    , validator_{std::make_unique<FretboardValidator>(scale_mgr_, instrument)}
    , note_gen_{std::make_unique<NoteGenerator>(*validator_)}
    , notes_{}
    , use_random_settings_{true} {}

void TablatureGenerator::generate() {
    if (use_random_settings_) {
        scale_mgr_.selectRandomKeyAndScale();
    }
    // Rebuild validator with current scale
    validator_ = std::make_unique<FretboardValidator>(scale_mgr_, instrument_);
    note_gen_ = std::make_unique<NoteGenerator>(*validator_);
    notes_ = note_gen_->generateTablature();
}

void TablatureGenerator::regenerate() {
    // Regenerate with same key/scale (don't call selectRandomKeyAndScale)
    // Rebuild validator with current scale
    validator_ = std::make_unique<FretboardValidator>(scale_mgr_, instrument_);
    note_gen_ = std::make_unique<NoteGenerator>(*validator_);
    notes_ = note_gen_->generateTablature();
}

void TablatureGenerator::setKeyAndScale(Music::KeyIndex key, const std::string& scale_name) {
    scale_mgr_.setKeyAndScale(key, scale_name);
    use_random_settings_ = false;
}

const std::vector<std::unique_ptr<Note>>& TablatureGenerator::getNotes() const noexcept {
    return notes_;
}

const Music::ScaleManager& TablatureGenerator::getScaleManager() const noexcept {
    return scale_mgr_;
}

InstrumentType TablatureGenerator::getInstrumentType() const noexcept {
    return instrument_;
}

Music::KeyIndex TablatureGenerator::getCurrentKeyIndex() const noexcept {
    return scale_mgr_.getCurrentKeyIndex();
}

std::string TablatureGenerator::getCurrentScaleName() const noexcept {
    return scale_mgr_.getCurrentScaleName();
}

} // namespace Guitar
