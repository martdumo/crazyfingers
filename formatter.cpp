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

void printTablature(const std::vector<std::unique_ptr<Note>>& notes) {
    for (int string_idx = 0; string_idx < NUM_STRINGS; ++string_idx) {
        std::cout << STRING_LABELS[string_idx] << "|";
        
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

} // namespace Formatter

} // namespace Guitar
