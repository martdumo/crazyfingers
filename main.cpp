// Crazy Fingers - Guitar/Bass Tablature Generator
// C++20 implementation with Music Theory Engine
// Interactive Menu with Advanced Options and Re-Roll Loop

#include "generator.h"
#include "formatter.h"
#include "easter_egg.h"
#include "music_theory.h"
#include <iostream>
#include <limits>
#include <string>

// ============================================================================
// Input Validation Helpers
// ============================================================================

// Clear input buffer on error
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Read integer with validation
bool readInteger(int& value) {
    if (!(std::cin >> value)) {
        clearInputBuffer();
        return false;
    }
    return true;
}

// Read string (single word, trimmed)
std::string readString() {
    std::string value;
    std::cin >> value;
    return value;
}

// ============================================================================
// Display Functions
// ============================================================================

void printHeader() {
    std::cout << "=== CRAZY FINGERS - MAIN MENU ===" << std::endl;
    std::cout << "[1] Generar Guitarra (Aleatorio)" << std::endl;
    std::cout << "[2] Generar Bajo (Aleatorio)" << std::endl;
    std::cout << "[3] Salir" << std::endl;
    std::cout << "[4] Opciones Avanzadas: Guitarra" << std::endl;
    std::cout << "[5] Opciones Avanzadas: Bajo" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Seleccione una opcion: ";
}

void printReRollMenu() {
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "[1] Generar otra variacion (Misma Tonalidad, Escala e Instrumento)" << std::endl;
    std::cout << "[2] Volver al Menu Principal" << std::endl;
    std::cout << "[3] Salir del Programa" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Seleccione una opcion: ";
}

void printScaleList() {
    const auto scales = Music::getScalesWithIds();
    std::cout << "\n=== ESCALAS DISPONIBLES ===" << std::endl;
    for (size_t i = 0; i < scales.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << scales[i] << std::endl;
    }
    std::cout << "============================" << std::endl;
}

void printSeparator() {
    std::cout << "\n========================================\n" << std::endl;
}

// ============================================================================
// Generation Functions
// ============================================================================

void displayTablature(const Guitar::TablatureGenerator& generator) {
    using namespace Guitar;
    
    int num_strings = (generator.getInstrumentType() == InstrumentType::Bass) 
                      ? BASS_NUM_STRINGS 
                      : GUITAR_NUM_STRINGS;
    
    Formatter::printTablature(generator.getNotes(), num_strings);
    
    const auto& scale_mgr = generator.getScaleManager();
    Formatter::printHarmonicInfo(
        scale_mgr.getCurrentKeyName(),
        scale_mgr.getCurrentScaleName(),
        scale_mgr.getScaleNotes()
    );
    
    std::cout << EasterEgg::generateAbsurdFact() << std::endl;
}

// Generate random tablature
void generateRandom(Guitar::InstrumentType instrument) {
    using namespace Guitar;
    
    TablatureGenerator generator(instrument);
    generator.generate();
    
    printSeparator();
    Formatter::printInstrumentInfo(instrument);
    std::cout << std::endl;
    displayTablature(generator);
    
    // Re-roll loop
    while (true) {
        printReRollMenu();
        int choice;
        if (!readInteger(choice)) {
            std::cout << "Entrada invalida. Intente de nuevo." << std::endl;
            continue;
        }
        
        if (choice == 1) {
            // Regenerate variation (same key/scale)
            generator.regenerate();
            printSeparator();
            displayTablature(generator);
        } else if (choice == 2) {
            // Return to main menu
            return;
        } else if (choice == 3) {
            // Exit program
            std::cout << "\nHasta luego! Keep rocking!" << std::endl;
            std::exit(0);
        } else {
            std::cout << "Opcion invalida. Intente de nuevo." << std::endl;
        }
    }
}

// Generate advanced tablature (user selects key and scale)
void generateAdvanced(Guitar::InstrumentType instrument) {
    using namespace Guitar;
    
    std::cout << "\n=== MODO AVANZADO ===" << std::endl;
    
    // Step 1: Get key from user
    std::cout << "Ingrese la tonalidad (ej: C, C#, D, D#, E, F, F#, G, G#, A, A#, B): ";
    std::string key_input = readString();
    int key_index = Music::parseKeyName(key_input);
    
    while (key_index == -1) {
        std::cout << "Tonalidad invalida. Intente de nuevo (C, C#, D, D#, E, F, F#, G, G#, A, A#, B): ";
        key_input = readString();
        key_index = Music::parseKeyName(key_input);
    }
    
    // Step 2: Display scale list
    printScaleList();
    
    // Step 3: Get scale from user
    std::cout << "Ingrese el NUMERO (ID) de la escala deseada: ";
    int scale_id;
    while (!readInteger(scale_id) || scale_id < 1) {
        std::cout << "ID invalido. Ingrese un numero valido: ";
        (void)readInteger(scale_id);  // Clear any remaining input
    }
    
    const auto scales = Music::getScalesWithIds();
    if (scale_id > static_cast<int>(scales.size())) {
        std::cout << "ID fuera de rango. Usando escala Major por defecto." << std::endl;
        scale_id = 1;
    }
    
    std::string selected_scale = scales[scale_id - 1];
    std::cout << "\nGenerando tablatura en " << Music::pitchClassToName(static_cast<Music::PitchClass>(key_index))
              << " " << selected_scale << "..." << std::endl;
    
    // Create generator and set specific key/scale
    TablatureGenerator generator(instrument);
    generator.setKeyAndScale(static_cast<Music::KeyIndex>(key_index), selected_scale);
    generator.generate();
    
    printSeparator();
    Formatter::printInstrumentInfo(instrument);
    std::cout << std::endl;
    displayTablature(generator);
    
    // Re-roll loop
    while (true) {
        printReRollMenu();
        int choice;
        if (!readInteger(choice)) {
            std::cout << "Entrada invalida. Intente de nuevo." << std::endl;
            continue;
        }
        
        if (choice == 1) {
            // Regenerate variation (same key/scale)
            generator.regenerate();
            printSeparator();
            displayTablature(generator);
        } else if (choice == 2) {
            // Return to main menu
            return;
        } else if (choice == 3) {
            // Exit program
            std::cout << "\nHasta luego! Keep rocking!" << std::endl;
            std::exit(0);
        } else {
            std::cout << "Opcion invalida. Intente de nuevo." << std::endl;
        }
    }
}

// ============================================================================
// Entry Point
// ============================================================================

int main() {
    using namespace Guitar;
    
    std::cout << "\n*** BIENVENIDO A CRAZY FINGERS ***" << std::endl;
    std::cout << "Generador de Tablaturas con Biomecanica Avanzada\n" << std::endl;
    
    while (true) {
        printHeader();
        
        int choice;
        if (!readInteger(choice)) {
            std::cout << "Entrada invalida. Intente de nuevo." << std::endl;
            continue;
        }
        
        if (choice == 1) {
            // Generate random guitar
            generateRandom(InstrumentType::Guitar);
        } else if (choice == 2) {
            // Generate random bass
            generateRandom(InstrumentType::Bass);
        } else if (choice == 3) {
            // Exit
            std::cout << "\nHasta luego! Keep rocking!" << std::endl;
            break;
        } else if (choice == 4) {
            // Advanced mode - Guitar
            generateAdvanced(InstrumentType::Guitar);
        } else if (choice == 5) {
            // Advanced mode - Bass
            generateAdvanced(InstrumentType::Bass);
        } else {
            std::cout << "Opcion invalida. Intente de nuevo." << std::endl;
        }
    }
    
    return 0;
}
