#include "fretboard.h"
#include <algorithm>
#include <cmath>

namespace Guitar {

// ============================================================================
// FretboardValidator Implementation
// ============================================================================

FretboardValidator::FretboardValidator(const Music::ScaleManager& scale_mgr, InstrumentType instrument)
    : scale_mgr_{scale_mgr}
    , instrument_{getInstrumentConfig(instrument)} {}

bool FretboardValidator::isNoteInScale(const Note& note) const {
    if (!note.isValid()) return false;
    return scale_mgr_.isMidiPitchValid(note.getMidiPitch());
}

bool FretboardValidator::isAnatomicallyPossible(const Note& from, const Note& to,
                                                  int max_fret_delta) const {
    if (!from.isValid() || !to.isValid()) return false;

    const int string_dist = std::abs(to.string_idx.value - from.string_idx.value);
    const int fret_dist = std::abs(to.fret.value - from.fret.value);

    // Only same or adjacent strings
    if (string_dist > 1) return false;

    // Max fret delta (default 3 for pivot movement)
    if (fret_dist > max_fret_delta) return false;

    // Must be different note
    if (string_dist == 0 && fret_dist == 0) return false;

    return true;
}

bool FretboardValidator::validateSlidingWindow(const Note& note1, const Note& note2,
                                                const Note& note3, int max_range) const {
    int min_fret = std::min({note1.fret.value, note2.fret.value, note3.fret.value});
    int max_fret = std::max({note1.fret.value, note2.fret.value, note3.fret.value});
    return (max_fret - min_fret) <= max_range;
}

std::vector<Note> FretboardValidator::getAllValidNotes() const {
    std::vector<Note> valid_notes;
    valid_notes.reserve(instrument_.num_strings * (MAX_FRET + 1) / 2);

    for (int s = 0; s < instrument_.num_strings; ++s) {
        for (int f = MIN_FRET; f <= MAX_FRET; ++f) {
            Note note{{s, instrument_.num_strings}, {f}};
            if (isNoteInScale(note)) {
                valid_notes.push_back(note);
            }
        }
    }

    return valid_notes;
}

} // namespace Guitar
