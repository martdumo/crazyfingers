#include "generator.h"
#include <algorithm>
#include <random>

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
    
    // Generate remaining notes
    int consecutive_same_string = 0;
    
    for (int i = 1; i < NUM_NOTES; ++i) {
        const Note& previous = *notes[i - 1];
        const bool must_change = (consecutive_same_string >= MAX_CONSECUTIVE_SAME_STRING);
        
        auto next_note = generateNextNote(previous, consecutive_same_string, 
                                          must_change, notes);
        
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
    note->string_idx.value = rng_.generateInt(1, 4);
    note->fret.value = rng_.generateInt(3, 12);
    
    // Ensure note is in scale
    int attempts = 0;
    while (!validator_.isNoteInScale(*note) && attempts < 50) {
        note->string_idx.value = rng_.generateInt(1, 4);
        note->fret.value = rng_.generateInt(3, 12);
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
    int consecutive_same_string,
    bool must_change_string,
    const std::vector<std::unique_ptr<Note>>& previous_notes
) {
    auto note = std::make_unique<Note>();
    bool found_valid = false;
    int attempts = 0;
    const int max_attempts = 100;
    
    // Phase 1: Random generation with constraints
    do {
        // String selection
        if (must_change_string || consecutive_same_string >= MAX_CONSECUTIVE_SAME_STRING) {
            // Force adjacent string change
            int direction = rng_.generateBool() ? -1 : 1;
            note->string_idx.value = previous.string_idx.value + direction;
            
            if (note->string_idx.value < 0) note->string_idx.value = 0;
            if (note->string_idx.value >= NUM_STRINGS) note->string_idx.value = NUM_STRINGS - 1;
            if (note->string_idx.value == previous.string_idx.value) {
                note->string_idx.value = (previous.string_idx.value == 0) ? 1 : previous.string_idx.value - 1;
            }
        } else {
            // Stay or move to adjacent
            int delta = rng_.generateInt(-1, 1);
            note->string_idx.value = previous.string_idx.value + delta;
            if (note->string_idx.value < 0) note->string_idx.value = 0;
            if (note->string_idx.value >= NUM_STRINGS) note->string_idx.value = NUM_STRINGS - 1;
        }
        
        // Fret selection (small deltas for pivot movement)
        int fret_delta = rng_.generateInt(-MAX_FRET_DELTA, MAX_FRET_DELTA);
        note->fret.value = previous.fret.value + fret_delta;
        if (note->fret.value < MIN_FRET) note->fret.value = MIN_FRET;
        if (note->fret.value > MAX_FRET) note->fret.value = MAX_FRET;
        
        // Validate
        found_valid = validator_.isNoteInScale(*note) &&
                      validator_.isAnatomicallyPossible(previous, *note, MAX_FRET_DELTA) &&
                      isValidForWindow(*note, previous_notes);
        
        attempts++;
    } while (!found_valid && attempts < max_attempts);
    
    // Phase 2: Systematic search
    if (!found_valid) {
        for (int str = previous.string_idx.value - 1; 
             str <= previous.string_idx.value + 1 && !found_valid; ++str) {
            if (str < 0 || str >= NUM_STRINGS) continue;
            if (must_change_string && str == previous.string_idx.value) continue;
            
            note->string_idx.value = str;
            
            for (int delta = -MAX_FRET_DELTA; delta <= MAX_FRET_DELTA && !found_valid; ++delta) {
                int test_fret = previous.fret.value + delta;
                if (test_fret < MIN_FRET || test_fret > MAX_FRET) continue;
                if (str == previous.string_idx.value && delta == 0) continue;
                
                note->fret.value = test_fret;
                
                if (validator_.isNoteInScale(*note) &&
                    validator_.isAnatomicallyPossible(previous, *note, MAX_FRET_DELTA) &&
                    isValidForWindow(*note, previous_notes)) {
                    found_valid = true;
                }
            }
        }
    }
    
    // Phase 3: Emergency fallback
    if (!found_valid) {
        note->string_idx.value = (previous.string_idx.value < 3) ? 
                                 previous.string_idx.value + 1 : previous.string_idx.value - 1;
        note->fret.value = previous.fret.value;
        
        if (note->string_idx.value < 0) note->string_idx.value = 0;
        if (note->string_idx.value >= NUM_STRINGS) note->string_idx.value = NUM_STRINGS - 1;
    }
    
    return note;
}

bool NoteGenerator::isValidForWindow(
    const Note& new_note,
    const std::vector<std::unique_ptr<Note>>& previous_notes
) const {
    if (previous_notes.size() < 2) return true;
    
    const Note& n1 = *previous_notes[previous_notes.size() - 2];
    const Note& n2 = *previous_notes[previous_notes.size() - 1];
    
    return validator_.validateSlidingWindow(n1, n2, new_note, MAX_WINDOW_RANGE);
}

// ============================================================================
// TablatureGenerator Implementation
// ============================================================================

TablatureGenerator::TablatureGenerator()
    : scale_mgr_{}
    , validator_{std::make_unique<FretboardValidator>(scale_mgr_)}
    , note_gen_{std::make_unique<NoteGenerator>(*validator_)}
    , notes_{} {}

void TablatureGenerator::generate() {
    notes_ = note_gen_->generateTablature();
}

const std::vector<std::unique_ptr<Note>>& TablatureGenerator::getNotes() const noexcept {
    return notes_;
}

const Music::ScaleManager& TablatureGenerator::getScaleManager() const noexcept {
    return scale_mgr_;
}

} // namespace Guitar
