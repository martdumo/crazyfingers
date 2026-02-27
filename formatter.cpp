#include "formatter.h"
#include <iostream>
#include <iomanip>

namespace Guitar {

// ============================================================================
// Formatter Implementation
// ============================================================================

namespace Formatter {

std::string formatNotePosition(const Note* note, int current_string) {
    if (note == nullptr) {
        return "---";
    }

    if (note->string_idx.value == current_string) {
        return std::to_string(note->fret.value);
    }

    return "---";
}

void printTablature(const std::vector<std::unique_ptr<Note>>& notes, int num_strings) {
    const auto& labels = (num_strings == GUITAR_NUM_STRINGS) 
                         ? GUITAR_STRING_LABELS 
                         : std::array<const char*, 6>{"G", "D", "A", "E", "", ""};

    for (int string_idx = 0; string_idx < num_strings; ++string_idx) {
        std::cout << labels[string_idx] << "|";

        for (const auto& note : notes) {
            const std::string position = formatNotePosition(note.get(), string_idx);

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

void printHarmonicInfo(const std::string& key_name,
                       const std::string& scale_name,
                       const std::string& scale_notes) {
    std::cout << std::endl;
    std::cout << key_name << " " << scale_name << " (" << scale_notes << ")" << std::endl;
}

void printInstrumentInfo(InstrumentType type) {
    if (type == InstrumentType::Bass) {
        std::cout << "[Bass Guitar - 4 strings, Standard Tuning (E1-A1-D2-G2)]" << std::endl;
    } else {
        std::cout << "[Electric Guitar - 6 strings, Standard Tuning (E2-A2-D3-G3-B3-E4)]" << std::endl;
    }
}

} // namespace Formatter

} // namespace Guitar
