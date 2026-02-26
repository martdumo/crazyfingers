# CrazyFingers - Guitar Tablature Generator
## Handoff Document (v3.0 - Extended Scale Dictionary)

---

## 1. Resumen del Proyecto

**CrazyFingers** es un generador profesional de tablaturas de guitarra que combina teoría musical avanzada con restricciones biomecánicas. Esta versión incluye **más de 70 escalas** de etnomusicología y teoría musical avanzada, garantizando coherencia armónica en cada generación.

### Tecnologías y Estándares

| Tecnología | Propósito |
|------------|-----------|
| **C++20** | Estándar moderno con `constexpr`, `std::unique_ptr`, `std::array` |
| **RAII** | Gestión automática de recursos [CG: R.1] |
| **`std::unique_ptr`** | Memoria dinámica segura [CG: R.11, C.149] |
| **`<random>` (std::mt19937)** | Generación de aleatoriedad de alta calidad |
| **C++ Core Guidelines** | Buenas prácticas de Bjarne Stroustrup |
| **Namespaces** | Organización lógica (`Music::`, `Guitar::`, `Formatter::`) [CG: SF.20] |

---

## 2. Arquitectura Modular

El proyecto está estructurado en **7 módulos independientes** (ninguno supera 200 líneas):

```
┌─────────────────────────────────────────────────────────────────┐
│                         main.cpp (~32 líneas)                   │
└─────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│                    generator.h/cpp (~83/~186 líneas)            │
│         (Coordinador + Generador de Notas con biomecánica)      │
└─────────────────────────────────────────────────────────────────┘
                                │
                ┌───────────────┼───────────────┐
                ▼               ▼               ▼
┌─────────────────────┐ ┌─────────────┐ ┌─────────────────────┐
│  music_theory.h/cpp │ │fretboard.h/ │ │ formatter.h/cpp     │
│  (~78/~105 líneas)  │ │   cpp       │ │ (~24/~47 líneas)    │
│  (Traductor notas)  │ │ ~84/~53     │ │ (Salida ASCII)      │
└─────────────────────┘ └─────────────┘ └─────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│           scale_dictionary.h/cpp (~38/~170 líneas)              │
│              (70+ escalas organizadas por categoría)            │
└─────────────────────────────────────────────────────────────────┘
```

### Archivos del Proyecto

| Archivo | Líneas | Responsabilidad |
|---------|--------|-----------------|
| `main.cpp` | 32 | Punto de entrada, orquestación |
| `music_theory.h/cpp` | 78 / 105 | Motor musical, traductor de notas |
| `scale_dictionary.h/cpp` | 38 / 170 | Diccionario de 70+ escalas |
| `fretboard.h/cpp` | 84 / 53 | Validación de notas, cálculo MIDI |
| `generator.h/cpp` | 83 / 186 | Generación con reglas biomecánicas |
| `formatter.h/cpp` | 24 / 47 | Formateo y salida ASCII a consola |

---

## 3. Instrucciones de Compilación y Ejecución

### Compilación

```bash
g++ -std=c++20 -Wall -Wextra -O2 -o crazyfingers.exe main.cpp music_theory.cpp scale_dictionary.cpp fretboard.cpp generator.cpp formatter.cpp
```

### Ejecución

```bash
crazyfingers.exe
```

El programa imprime la tablatura ASCII y la información armónica, luego termina.

---

## 4. Muestra de Salida

```
e|---------------------------------4---2---4---2-----------5------|
B|-----6---9-----------6---9---6-------------------3---2-------3--|
G|-7-----------10--7----------------------------------------------|
D|----------------------------------------------------------------|
A|----------------------------------------------------------------|
E|----------------------------------------------------------------|

F# Harmonic Minor (F# G# A B C# D F F#)
```

---

## 5. Motor de Teoría Musical Expandido

### Tonalidades (12)
C, C#, D, D#, E, F, F#, G, G#, A, A#, B

### Escalas Disponibles (70+)

#### Comunes/Modos (15)
Major, Harmonic Minor, Melodic Minor, Natural Minor, Pentatonic Major, Pentatonic Minor, Pentatonic Blues, Pentatonic Neutral, Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian

#### Simétricas/Alteradas (16)
Chromatic, Whole Tone, Octatonic (H-W), Octatonic (W-H), Augmented, Altered, Diatonic, Diminished, Diminished Half, Diminished Whole, Diminished Whole Tone, Dominant 7th, Lydian Augmented, Lydian Minor, Lydian Diminished, Half Diminished

#### Jazz/Bebop (8)
Bebop Major, Bebop Minor, Bebop Dominant, Bebop Half Diminished, Blues, Major Blues Scale, Dominant Pentatonic, Mixo-Blues

#### Exóticas y del Mundo (41)
Algerian, Arabian #1, Arabian #2, Balinese, Byzantine, Chinese, Chinese Mongolian, Egyptian, Eight Tone Spanish, Ethiopian (A raray), Ethiopian (Geez&Ezel), Hawaiian, Hindu, Hindustan, Hirajoshi, Hungarian Major, Hungarian Gypsy, Hungarian Minor, Japanese #1, Japanese #2, Javaneese, Jewish (Adonai Malakh), Jewish (Ahaba Rabba), Kumoi, Mohammedan, Neopolitan, Neopolitan Major, Neopolitan Minor, Oriental #1, Oriental #2, Pelog, Persian, Prometheus, Prometheus Neopolitan, Roumanian Minor, Spanish Gypsy, Super Locrian, Iwato, Moorish Phrygian, Double Harmonic, Enigmatic

### Traductor de Notas

El sistema convierte automáticamente la tonalidad + intervalos en nombres de notas:

```cpp
// Ejemplo: Key 0 (C) + Major intervals = "C D E F G A B"
// Ejemplo: Key 6 (F#) + Hirajoshi = "F# G# A C# D"
```

---

## 6. Reglas Biomecánicas

| Regla | Valor | Propósito |
|-------|-------|-----------|
| **Máx notas consecutivas misma cuerda** | 3 | Obliga a moverse por el mástil |
| **Distancia máx entre notas consecutivas** | 3 trastes | Movimiento de pivote |
| **Ventana deslizante (3 notas)** | Máx 5 trastes | Alcance real de 4 dedos |
| **Cuerdas adyacentes** | ±1 | Movimiento ergonómico |

---

## 7. C++ Core Guidelines Aplicadas

| Guideline | Aplicación |
|-----------|------------|
| [CG: R.1] RAII | `TablatureGenerator` gestiona recursos |
| [CG: R.11] No naked new/delete | `std::unique_ptr<Note>` |
| [CG: C.149] Smart pointers | `std::vector<std::unique_ptr<Note>>` |
| [CG: F.4] Constexpr | Constantes compiladas |
| [CG: F.2] Single purpose | Funciones con único propósito |
| [CG: SF.20] Namespaces | `Music::`, `Guitar::`, `Formatter::` |
| [CG: SF.3] Headers for interfaces | `.h` para declaraciones |
| [CG: SF.5] Implementation in .cpp | Lógica en `.cpp` |
| [CG: C.61] Copy semantics | No copiable, sí movable |

---

## 8. Flujo de Generación

```
1. ScaleDictionary::getInstance()
   └─► Inicializa 70+ escalas en unordered_map

2. ScaleManager::selectRandomKeyAndScale()
   └─► Elige tonalidad (0-11) y escala aleatoria
   └─► Calcula pitch classes y nombres de notas

3. FretboardValidator::getAllValidNotes()
   └─► Escanea diapasón (6 cuerdas × 23 trastes)
   └─► Filtra notas de la escala seleccionada

4. NoteGenerator::generateTablature()
   └─► Genera 16 notas con restricciones biomecánicas
   └─► Valida ventana deslizante y escala musical

5. Formatter::printTablature() + printHarmonicInfo()
   └─► Imprime tablatura ASCII
   └─► Imprime: "Key Escala (Nota1 Nota2 ...)"
```

---

**Proyecto concluido exitosamente.** 🎸

CrazyFingers v3.0: 70+ escalas, traductor de notas, arquitectura modular profesional.
