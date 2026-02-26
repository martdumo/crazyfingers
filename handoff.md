# CrazyFingers - Guitar Tablature Generator
## Handoff Document (v4.0 - Final con Humor Negro)

---

## 1. Resumen del Proyecto

**CrazyFingers** es un generador profesional de tablaturas de guitarra que combina teoria musical avanzada con restricciones biomecanicas y un toque de humor absurdo. Esta version incluye **mas de 70 escalas** de etnomusicologia y **125,000 curiosidades absurdas** posibles.

### Tecnologias y Estandares

| Tecnologia | Proposito |
|------------|-----------|
| **C++20** | Estandar moderno con `constexpr`, `std::unique_ptr`, `std::array` |
| **RAII** | Gestion automatica de recursos [CG: R.1] |
| **`std::unique_ptr`** | Memoria dinamica segura [CG: R.11, C.149] |
| **`<random>` (std::mt19937)** | Generacion de aleatoriedad de alta calidad |
| **`std::discrete_distribution`** | Seleccion ponderada organica |
| **C++ Core Guidelines** | Buenas practicas de Bjarne Stroustrup |
| **Namespaces** | Organizacion logica (`Music::`, `Guitar::`, `EasterEgg::`) [CG: SF.20] |

---

## 2. Arquitectura Modular

El proyecto esta estructurado en **8 modulos independientes** (ninguno supera ~300 lineas):

```
┌─────────────────────────────────────────────────────────────────┐
│                         main.cpp (~40 lineas)                   │
└─────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│                    generator.h/cpp (~110/~290 lineas)           │
│         (Generador organico con pesos probabilisticos)          │
└─────────────────────────────────────────────────────────────────┘
                                │
                ┌───────────────┼───────────────┐
                ▼               ▼               ▼
┌─────────────────────┐ ┌─────────────┐ ┌─────────────────────┐
│  music_theory.h/cpp │ │fretboard.h/ │ │ formatter.h/cpp     │
│  (~78/~105 lineas)  │ │   cpp       │ │ (~24/~47 lineas)    │
│  (Traductor notas)  │ │ ~84/~53     │ │ (Salida ASCII)      │
└─────────────────────┘ └─────────────┘ └─────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│           scale_dictionary.h/cpp (~38/~170 lineas)              │
│              (70+ escalas organizadas por categoria)            │
└─────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│              easter_egg.h/cpp (~150/~30 lineas)                 │
│         (125,000 curiosidades absurdas con humor negro)         │
└─────────────────────────────────────────────────────────────────┘
```

### Archivos del Proyecto

| Archivo | Lineas | Responsabilidad |
|---------|--------|-----------------|
| `main.cpp` | ~40 | Punto de entrada, orquestacion |
| `music_theory.h/cpp` | ~78 / ~105 | Motor musical, traductor de notas |
| `scale_dictionary.h/cpp` | ~38 / ~170 | Diccionario de 70+ escalas |
| `fretboard.h/cpp` | ~84 / ~53 | Validacion de notas, calculo MIDI |
| `generator.h/cpp` | ~110 / ~290 | Generacion organica con pesos |
| `formatter.h/cpp` | ~24 / ~47 | Formateo y salida ASCII |
| `easter_egg.h/cpp` | ~150 / ~30 | Curiosidades absurdas (50x50x50) |
| `handoff.md` | ~260 | Documentacion completa |

---

## 3. Instrucciones de Compilacion y Ejecucion

### Compilacion

```bash
g++ -std=c++20 -Wall -Wextra -O2 -o crazyfingers.exe main.cpp music_theory.cpp scale_dictionary.cpp fretboard.cpp generator.cpp formatter.cpp easter_egg.cpp
```

**Explicacion de flags:**
| Flag | Proposito |
|------|-----------|
| `-std=c++20` | Estandar C++20 requerido |
| `-Wall` | Todas las advertencias comunes |
| `-Wextra` | Advertencias adicionales |
| `-O2` | Optimizacion de nivel 2 |
| `-o crazyfingers.exe` | Nombre del binario |

### Ejecucion

```bash
crazyfingers.exe
```

El programa imprime la tablatura ASCII, la informacion armonica y una curiosidad absurda, luego termina.

---

## 4. Muestra de Salida

```
e|---------------------------------15--13--11--10------10--11-----|
B|---------13------------------13------------------11----------13-|
G|-8---10------12--14------15-------------------------------------|
D|---------------------13-----------------------------------------|
A|----------------------------------------------------------------|
E|----------------------------------------------------------------|

A# Mohammedan (A# C D D# F G A A#)
Sabia usted que un vikingo vegano afino su guitarra usando telequinesis en una crisis de mediana edad?
```

---

## 5. Motor de Teoria Musical Expandido

### Tonalidades (12)
C, C#, D, D#, E, F, F#, G, G#, A, A#, B

### Escalas Disponibles (70+)

#### Comunes/Modos (15)
Major, Harmonic Minor, Melodic Minor, Natural Minor, Pentatonic Major, Pentatonic Minor, Pentatonic Blues, Pentatonic Neutral, Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian

#### Simetricas/Alteradas (16)
Chromatic, Whole Tone, Octatonic (H-W), Octatonic (W-H), Augmented, Altered, Diatonic, Diminished, Diminished Half, Diminished Whole, Diminished Whole Tone, Dominant 7th, Lydian Augmented, Lydian Minor, Lydian Diminished, Half Diminished

#### Jazz/Bebop (8)
Bebop Major, Bebop Minor, Bebop Dominant, Bebop Half Diminished, Blues, Major Blues Scale, Dominant Pentatonic, Mixo-Blues

#### Exoticas y del Mundo (41)
Algerian, Arabian #1, Arabian #2, Balinese, Byzantine, Chinese, Chinese Mongolian, Egyptian, Eight Tone Spanish, Ethiopian (A raray), Ethiopian (Geez&Ezel), Hawaiian, Hindu, Hindustan, Hirajoshi, Hungarian Major, Hungarian Gypsy, Hungarian Minor, Japanese #1, Japanese #2, Javaneese, Jewish (Adonai Malakh), Jewish (Ahaba Rabba), Kumoi, Mohammedan, Neopolitan, Neopolitan Major, Neopolitan Minor, Oriental #1, Oriental #2, Pelog, Persian, Prometheus, Prometheus Neopolitan, Roumanian Minor, Spanish Gypsy, Super Locrian, Iwato, Moorish Phrygian, Double Harmonic, Enigmatic

### Traductor de Notas

El sistema convierte automaticamente la tonalidad + intervalos en nombres de notas:

```cpp
// Ejemplo: Key 0 (C) + Major intervals = "C D E F G A B"
// Ejemplo: Key 6 (F#) + Hirajoshi = "F# G# A C# D"
```

---

## 6. Reglas Biomecanicas y Generacion Organica

### Restricciones Fisicas

| Regla | Valor | Proposito |
|-------|-------|-----------|
| **Max notas consecutivas misma cuerda** | 2-3 | Obliga a moverse, permite asentar |
| **Distancia max absoluta** | 4 trastes | Solo como "nota sorpresa" |
| **Ventana deslizante (4 notas)** | Max 5 trastes | Alcance real de 4 dedos |
| **Cuerdas adyacentes** | +-1 | Movimiento ergonomico |

### Sistema de Pesos Probabilisticos

El generador usa `std::discrete_distribution` para seleccionar notas con probabilidades ponderadas:

| Distancia (trastes) | Peso | Probabilidad | Descripcion |
|---------------------|------|--------------|-------------|
| 0-2 | 70 | ~70% | **Muy comodo** - grados conjuntos |
| 3 | 20 | ~20% | **Moderado** - estiramiento comun |
| 4 | 10 | ~10% | **Sorpresa** - color ocasional |
| >=5 | 0 | 0% | **Prohibido** - previene lesiones |

### Flujo de Generacion Organica

```
1. Construir candidatos validos (escala + ventana deslizante)
2. Asignar pesos segun distancia de trastes
3. Bonus +30% si permanece en misma cuerda (fluidez)
4. Seleccionar con std::discrete_distribution
5. Resultado: fraseos naturales con color ocasional
```

**Ejemplo de secuencia organica:**
```
G|-7---5-------9-------5---7---9---11-------------|  <- Movimiento natural
D|---------7-------7-------------------------------|
```

**vs. secuencia "quiebra-muñecas" (evitada):**
```
G|-12------5---------------------------------------|  <- Salto de 7 trastes (PROHIBIDO)
D|---------12--------------------------------------|
```

---

## 7. Curiosidad Absurda (Easter Egg)

Al final de cada ejecucion, el programa genera una curiosidad aleatoria combinando:
- **50 Sujetos** (desde George Bush hasta "el autor de este codigo", pasando por zombies y demonios)
- **50 Acciones** (desde "aprendio a tocar la guitarra" hasta "matio a alguien con una guitarra")
- **50 Motivos** (desde "para lidiar mejor con su suegra" hasta "en sus ultimos minutos de vida")

**Total de combinaciones posibles:** 50 x 50 x 50 = **125,000** curiosidades unicas.

**Nota:** Todo el texto usa solo caracteres ASCII sin acentos para maxima compatibilidad con consolas.

### Ejemplos de Salida (Humor Negro Incluido)

```
Sabia usted que un zombie con depresion robo la pua del destino para curar su insomnio?

Sabia usted que Dracula toco mientras el mundo se incendiaba para comunicarse con los delfines?

Sabia usted que el autor de este codigo uso cuerdas de alambre de puas en un arrebato de locura temporal?

Sabia usted que un banquero de Wall Street fundo una banda de death metal mientras escapaba de la policia?

Sabia usted que la Parca con resaca sacrifico su gato para afinar mejor porque le debia dinero a la mafia?

Sabia usted que un politico corrupto se electrocuto por un solo epico para evitar la carcel?

Sabia usted que un vampiro vegetariano toco un solo postumo desde el mas alla en venganza contra la humanidad?
```

### Implementacion

```cpp
// easter_egg.h/cpp
- 3 x std::array<const char*, 50>
- std::mt19937 para seleccion aleatoria
- Formato: "Sabia usted que [Sujeto] [Accion] [Motivo]?"
- Sin caracteres acentuados (ASCII-safe)
```

---

## 8. C++ Core Guidelines Aplicadas

| Guideline | Aplicacion |
|-----------|------------|
| [CG: R.1] RAII | `TablatureGenerator` gestiona recursos |
| [CG: R.11] No naked new/delete | `std::unique_ptr<Note>` |
| [CG: C.149] Smart pointers | `std::vector<std::unique_ptr<Note>>` |
| [CG: F.4] Constexpr | Constantes compiladas |
| [CG: F.2] Single purpose | Funciones con unico proposito |
| [CG: SF.20] Namespaces | `Music::`, `Guitar::`, `EasterEgg::` |
| [CG: SF.3] Headers for interfaces | `.h` para declaraciones |
| [CG: SF.5] Implementation in .cpp | Logica en `.cpp` |
| [CG: C.61] Copy semantics | No copiable, si movable |

---

## 9. Flujo de Generacion

```
1. ScaleDictionary::getInstance()
   +-- Inicializa 70+ escalas en unordered_map

2. ScaleManager::selectRandomKeyAndScale()
   +-- Elige tonalidad (0-11) y escala aleatoria
   +-- Calcula pitch classes y nombres de notas

3. FretboardValidator::getAllValidNotes()
   +-- Escanea diapasón (6 cuerdas x 23 trastes)
   +-- Filtra notas de la escala seleccionada

4. NoteGenerator::generateTablature()
   +-- Construye candidatos validos
   +-- Asigna pesos (70/20/10) por distancia
   +-- Selecciona con std::discrete_distribution
   +-- Valida ventana deslizante (4 notas, max 5 trastes)

5. Formatter::printTablature() + printHarmonicInfo()
   +-- Imprime tablatura ASCII
   +-- Imprime: "Key Escala (Nota1 Nota2 ...)"

6. EasterEgg::generateAbsurdFact()
   +-- Selecciona sujeto, accion y motivo aleatorios
   +-- Imprime: "Sabia usted que [Sujeto] [Accion] [Motivo]?"
```

---

## 10. Estructura Final de Archivos

```
crazyfingers/
├── main.cpp                (~40 lineas)  - Punto de entrada
├── music_theory.h/cpp      (~78/~105)    - Motor musical + traductor
├── scale_dictionary.h/cpp  (~38/~170)    - 70+ escalas
├── fretboard.h/cpp         (~84/~53)     - Validacion de fretboard
├── generator.h/cpp         (~110/~290)   - Generador organico ponderado
├── formatter.h/cpp         (~24/~47)     - Salida ASCII
├── easter_egg.h/cpp        (~150/~30)    - 125,000 curiosidades
├── handoff.md              (~260)        - Documentacion
└── crazyfingers.exe        - Binario compilado
```

**Total lineas de codigo:** ~1,000 lineas distribuidas en 8 modulos

---

**Proyecto concluido exitosamente.** 🎸

CrazyFingers v4.0: 70+ escalas, generacion organica ponderada, 125,000 curiosidades absurdas, arquitectura modular profesional, humor negro opcional.
