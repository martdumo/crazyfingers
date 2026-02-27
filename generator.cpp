#include "generator.h"
#include <algorithm>

namespace Guitar {

// ============================================================================
// PositionBox Implementation
// ============================================================================

void PositionBox::initialize(int first_fret) {
    anchor_fret = first_fret;
    min_fret = std::max(MIN_FRET, anchor_fret - POSITION_BOX_RADIUS);
    max_fret = std::min(MAX_FRET, anchor_fret + POSITION_BOX_RADIUS);
}

// ============================================================================
// NoteGenerator Implementation
// ============================================================================

NoteGenerator::NoteGenerator(const FretboardValidator& validator)
    : validator_{validator}
    , rng_{}
    , valid_notes_cache_{validator.getAllValidNotes()}
    , position_box_{} {}

std::vector<std::unique_ptr<Note>> NoteGenerator::generateTablature() {
    std::vector<std::unique_ptr<Note>> notes;
    notes.reserve(NUM_NOTES);

    // Generate first note and initialize Position Box
    auto first_note = generateFirstNote();
    position_box_.initialize(first_note->fret.value);
    notes.push_back(std::move(first_note));

    // Generate remaining notes within Position Box
    int consecutive_same_string = 0;

    for (int i = 1; i < NUM_NOTES; ++i) {
        const Note& previous = *notes[i - 1];

        // Force string change after 3 consecutive notes on same string
        // (passed internally to buildCandidates via must_change_string logic)
        const bool must_change_string = (consecutive_same_string >= MAX_CONSECUTIVE_SAME_STRING);

        auto next_note = generateNextNote(previous, consecutive_same_string, must_change_string, notes);

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
    const std::vector<std::unique_ptr<Note>>& /* previous_notes */
) {
    // Build list of valid candidates with weights
    auto candidates = buildCandidates(previous, must_change_string, position_box_);

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
    const PositionBox& box
) {
    std::vector<NoteCandidate> candidates;
    const int num_strings = validator_.getInstrument().num_strings;

    // Determine which strings to consider
    std::vector<int> strings_to_check;

    if (must_change_string) {
        // Must change string - but can skip to ANY string (free string skipping)
        for (int s = 0; s < num_strings; ++s) {
            if (s != previous.string_idx.value) {
                strings_to_check.push_back(s);
            }
        }
    } else {
        // Can stay on same string or jump to any other string
        for (int s = 0; s < num_strings; ++s) {
            strings_to_check.push_back(s);
        }
    }

    // For each string, find valid fret candidates within Position Box
    for (int str : strings_to_check) {
        // Check all frets within the Position Box
        for (int fret = box.min_fret; fret <= box.max_fret; ++fret) {
            // Skip if same note as previous (must be different)
            if (str == previous.string_idx.value && fret == previous.fret.value) continue;

            Note candidate_note{{str, num_strings}, {fret}};

            // Check if note is in scale
            if (!validator_.isNoteInScale(candidate_note)) continue;

            // Calculate fret distance from previous note for weighting
            int fret_distance = std::abs(fret - previous.fret.value);

            // Calculate weight based on distance
            int weight = calculateWeight(fret_distance);

            // Bonus for staying on same string (promotes fluency when possible)
            if (str == previous.string_idx.value) {
                weight = static_cast<int>(weight * 1.2);  // 20% bonus
            }

            candidates.push_back({candidate_note, weight, fret_distance});
        }
    }

    return candidates;
}

int NoteGenerator::calculateWeight(int fret_distance) const {
    // Weight system based on comfort level
    switch (fret_distance) {
        case 0:
            // Same fret, different string (very comfortable)
            return WEIGHT_CLOSE;
        case 1:
            // 1 fret distance (very comfortable)
            return WEIGHT_CLOSE;
        case 2:
            // 2 frets distance (comfortable)
            return WEIGHT_CLOSE;
        case 3:
            // 3 frets distance (moderate)
            return WEIGHT_MEDIUM;
        case 4:
            // 4 frets distance (stretch - use sparingly)
            return WEIGHT_FAR;
        default:
            // Should not reach here within Position Box
            return 0;
    }
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
