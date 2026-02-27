// Crazy Fingers - Guitar/Bass Tablature Generator
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

    // FEATURE 1: Instrument Selection
    std::cout << "=== Crazy Fingers - Tablature Generator ===" << std::endl;
    std::cout << "Seleccione instrumento: [1] Guitarra (6 cuerdas)  [2] Bajo (4 cuerdas): ";
    
    int instrument_choice;
    std::cin >> instrument_choice;
    
    InstrumentType instrument = (instrument_choice == 2) 
                                ? InstrumentType::Bass 
                                : InstrumentType::Guitar;

    // Print instrument info
    Formatter::printInstrumentInfo(instrument);
    std::cout << std::endl;

    // Create generator (RAII - automatic cleanup on scope exit)
    TablatureGenerator generator(instrument);

    // Generate tablature with music theory constraints
    generator.generate();

    // Print tablature to console
    Formatter::printTablature(generator.getNotes(), 
                              (instrument == InstrumentType::Bass) ? BASS_NUM_STRINGS : GUITAR_NUM_STRINGS);

    // Print harmonic inspiration with scale notes
    // Format: "C Major (C D E F G A B)"
    const auto& scale_mgr = generator.getScaleManager();
    Formatter::printHarmonicInfo(
        scale_mgr.getCurrentKeyName(),
        scale_mgr.getCurrentScaleName(),
        scale_mgr.getScaleNotes()
    );

    // Print absurd Easter egg fact
    // Format: "Sabia usted que [Sujeto] [Accion] [Motivo]?"
    std::cout << EasterEgg::generateAbsurdFact() << std::endl;

    return 0;
}
