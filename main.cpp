// Crazy Fingers - Guitar Tablature Generator
// C++20 implementation with Music Theory Engine
// Following C++ Core Guidelines

#include "generator.h"
#include "formatter.h"
#include "easter_egg.h"
#include <iostream>

// ============================================================================
// Entry Point [CG: R.1 - RAII cleanup]
// ============================================================================

int main() {
    using namespace Guitar;
    
    // Create generator (RAII - automatic cleanup on scope exit)
    TablatureGenerator generator;
    
    // Generate tablature with music theory constraints
    generator.generate();
    
    // Print tablature to console
    Formatter::printTablature(generator.getNotes());
    
    // Print harmonic inspiration with scale notes
    // Format: "C Major (C D E F G A B)"
    const auto& scale_mgr = generator.getScaleManager();
    Formatter::printHarmonicInfo(
        scale_mgr.getCurrentKeyName(),
        scale_mgr.getCurrentScaleName(),
        scale_mgr.getScaleNotes()
    );
    
    // Print absurd Easter egg fact
    // Format: "Sabía usted que [Sujeto] [Acción] [Motivo]?"
    std::cout << EasterEgg::generateAbsurdFact() << std::endl;
    
    return 0;  // Immediate exit, no user input
}
