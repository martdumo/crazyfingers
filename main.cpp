// Crazy Fingers - Guitar Tablature Generator
// C++20 implementation with Music Theory Engine
// Following C++ Core Guidelines

#include "generator.h"
#include "formatter.h"
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
    
    // Print harmonic inspiration
    Formatter::printHarmonicInfo(generator.getScaleManager().getFullDescription());
    
    return 0;  // Immediate exit, no user input
}
