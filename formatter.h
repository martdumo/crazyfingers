#ifndef FORMATTER_H
#define FORMATTER_H

#include <vector>
#include <memory>
#include <string>
#include "fretboard.h"

namespace Guitar {

// ============================================================================
// Tablature Formatter
// ============================================================================

namespace Formatter {

constexpr int NOTE_WIDTH = 4;

// Format a single note position for a given string
[[nodiscard]] std::string formatNotePosition(const Note* note, int current_string);

// Print complete tablature to console
void printTablature(const std::vector<std::unique_ptr<Note>>& notes);

// Print harmonic info with scale notes
// Format: "C Major (C D E F G A B)"
void printHarmonicInfo(const std::string& key_name, 
                       const std::string& scale_name,
                       const std::string& scale_notes);

} // namespace Formatter

} // namespace Guitar

#endif // FORMATTER_H
