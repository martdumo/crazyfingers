# Crazy Fingers - Guitar/Bass Tablature Generator
## Blueprint de Arquitectura C++20

---

## 1. Arquitectura del Software

### 1.1 Visión General

El sistema sigue un diseño **modular y orientado a componentes** con separación clara de responsabilidades:

```
┌─────────────────────────────────────────────────────────────┐
│                      main.cpp                               │
│                    (Entry Point + Instrument Select)        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                  TablatureGenerator                         │
│         (Coordinador principal - RAII, Instrument-aware)    │
└─────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┼───────────────┐
              ▼               ▼               ▼
┌─────────────────────┐ ┌─────────────┐ ┌─────────────────────┐
│   NoteGenerator     │ │FretboardVal.│ │   TabFormatter      │
│  (Aleatoriedad +    │ │ (Validación │ │   (Salida consola   │
│   Inercia)          │ │  dinámica)  │ │    4 o 6 líneas)    │
└─────────────────────┘ └─────────────┘ └─────────────────────┘
```

### 1.2 Principios de Diseño Aplicados

| Principio | Aplicación |
|-----------|------------|
| **RAII** [CG: R.1] | `TablatureGenerator` gestiona recursos en su ciclo de vida |
| **Single Responsibility** [CG: F.2] | Cada clase tiene un propósito único |
| **Constexpr** [CG: F.4] | Constantes de guitarra/bajo evaluadas en compile-time |
| **No naked new/delete** [CG: R.11] | `std::vector` y `std::unique_ptr` para memoria dinámica |
| **Type Safety** | `struct Note` con validación de instrumento |
| **Open/Closed** | `InstrumentType` enum permite extensión futura |

---

## 2. Lógica del Algoritmo Paso a Paso

### 2.1 Constantes del Dominio (Compile-Time)

```
GUITAR_NUM_STRINGS = 6
BASS_NUM_STRINGS   = 4
NUM_FRETS          = 22  (0-22 inclusive)
NUM_NOTES          = 16
MAX_FRET_DELTA     = 4   (distancia máxima entre trastes consecutivos)
SLIDING_WINDOW     = 3   (notas N, N-1, N-2)
MAX_WINDOW_RANGE   = 5   (alcance máximo de la mano en ventana de 3 notas)
```

### 2.2 Mapeo de Cuerdas (Notación Estándar)

#### Guitarra (6 cuerdas)
| Índice | Cuerda | Nota al aire | MIDI |
|--------|--------|--------------|------|
| 0 | 1ra (aguda) | E (Mi alto) | 64 |
| 1 | 2da | B (Si) | 59 |
| 2 | 3ra | G (Sol) | 55 |
| 3 | 4ta | D (Re) | 50 |
| 4 | 5ta | A (La) | 45 |
| 5 | 6ta (grave) | E (Mi bajo) | 40 |

#### Bajo (4 cuerdas)
| Índice | Cuerda | Nota al aire | MIDI |
|--------|--------|--------------|------|
| 0 | 1ra (aguda) | G (Sol) | 43 |
| 1 | 2da | D (Re) | 38 |
| 2 | 3ra | A (La) | 33 |
| 3 | 4ta (grave) | E (Mi bajo) | 28 |

### 2.3 Algoritmo de Generación con Inercia

```
ALGORITMO: GenerateTablatureWithInertia
┌─────────────────────────────────────────────────────────────────┐
│ 1. INICIALIZACIÓN                                               │
│    - Seleccionar instrumento (Guitarra/Bajo)                    │
│    - Crear vector de 16 notas (std::vector<Note>)               │
│    - Inicializar generador aleatorio con seed temporal          │
│    - Inicializar MovementHistory (vacío)                        │
│                                                                 │
│ 2. PRIMERA NOTA (aleatoria pura)                                │
│    - string_idx = random(1, num_strings-2)  [cuerdas centrales] │
│    - fret = random(5, 12)  [posición media]                     │
│    - notes[0] = Note{string_idx, fret}                          │
│                                                                 │
│ 3. GENERAR 15 NOTAS RESTANTES (con inercia)                     │
│    FOR i = 1 TO 15:                                             │
│                                                                 │
│      a) SELECCIÓN DE CUERDA (adyacente o misma)                 │
│         prev_string = notes[i-1].string                         │
│         delta_string = random(-1, 0, 1)                         │
│         new_string = clamp(prev_string + delta_string)          │
│                                                                 │
│      b) SELECCIÓN DE TRASTE (máx 4 trastes de distancia)        │
│         prev_fret = notes[i-1].fret                             │
│         delta_fret = random(-4, 4)                              │
│         new_fret = clamp(prev_fret + delta_fret, 0, 22)         │
│                                                                 │
│      c) VALIDACIÓN DE TOCABILIDAD                               │
│         - Verificar nota en escala                              │
│         - Verificar ventana deslizante (3 notas, max 5 trastes) │
│         - Calcular peso base por distancia                      │
│         - Aplicar modificador de inercia                        │
│                                                                 │
│      d) INERCIA DE MOVIMIENTO                                   │
│         SI |last_delta| >= 3 (salto grande anterior):           │
│           * Bonus +40 si |delta_fret| <= 1 (asentar mano)       │
│           * Penalización -50 si zigzag (delta opuesto grande)   │
│           * Pequeña penalización -10 si salto consecutivo      │
│                                                                 │
│      e) ALMACENAR                                               │
│         notes[i] = Note{new_string, new_fret}                   │
│         history.last_fret_delta = prev_fret - prev_prev_fret    │
│                                                                 │
│ 4. FORMATEO DE SALIDA                                           │
│    - Construir N líneas (4 para bajo, 6 para guitarra)          │
│    - Para cada posición (0-15):                                 │
│      * Colocar número de traste en la cuerda correspondiente    │
│      * Poner '---' en las demás cuerdas                         │
│    - Imprimir en orden descendente (grave → aguda)              │
│                                                                 │
│ 5. TERMINACIÓN                                                  │
│    - Imprimir Tonalidad, Escala y notas                         │
│    - Imprimir frase absurda de easter_egg                       │
│    - Destructor libera recursos automáticamente (RAII)          │
└─────────────────────────────────────────────────────────────────┘
```

### 2.4 Reglas de Inercia Biomecánica

| Situación | Regla | Peso |
|-----------|-------|------|
| Salto grande anterior (≥3 trastes) | Movimiento pequeño (0-1 trastes) | +40 bonus |
| Salto grande anterior | Zigzag agresivo (dirección opuesta ≥3) | -50 penalización |
| Salto grande anterior | Otro salto grande misma dirección | -10 penalización |
| Sin salto grande anterior | Peso normal por distancia | sin cambio |

### 2.5 Ventana Deslizante Estricta (3 Notas)

```
NOTAS:     N-2    N-1     N
TRASTES:   7      9      10   ← VÁLIDO (max-min = 10-7 = 3 ≤ 5)

NOTAS:     N-2    N-1     N
TRASTES:   5      12      8   ← INVÁLIDO (max-min = 12-5 = 7 > 5)
```

**Regla:** En cualquier grupo de 3 notas consecutivas, la diferencia entre el traste más alto y el más bajo NO DEBE exceder 5 trastes. Esto representa que la mano permanece en su "caja" o posición actual.

### 2.6 Restricciones de Tocabilidad

| Restricción | Valor | Justificación |
|-------------|-------|---------------|
| Salto máximo de trastes | 4 | Evita estiramientos imposibles |
| Cuerdas adyacentes | ±1 | Movimiento natural del brazo |
| Ventana 3 notas | max 5 trastes | Alcance real de 4 dedos en posición |
| Inercia post-salto | preferencia 0-1 trastes | "Asentar" la mano después de estiramiento |
| Zigzag agresivo | penalizado | Evita movimientos antinaturales |

---

## 3. Estructura de Clases/Structs

### 3.1 `enum class InstrumentType`

```cpp
enum class InstrumentType {
    Guitar,  // 6 strings, standard tuning (E2-A2-D3-G3-B3-E4)
    Bass     // 4 strings, standard tuning (E1-A1-D2-G2)
};
```

### 3.2 `struct Note`

```cpp
struct Note {
    StringIndex string_idx;  // value (0-N), num_strings (total)
    Fret fret;               // 0-22

    [[nodiscard]] int getMidiPitch() const noexcept;
    // Guitar: OPEN_STRING_MIDI[string] + fret
    // Bass:   BASS_OPEN_STRING_MIDI[string] + fret

    [[nodiscard]] PitchClass getPitchClass() const noexcept;
    // midi_pitch % 12
};
```

### 3.3 `struct MovementHistory`

```cpp
struct MovementHistory {
    std::optional<int> last_fret_delta;   // N-1 to N-2 movement
    std::optional<int> last_string_delta; // String change direction

    [[nodiscard]] bool wasBigJump() const;  // |delta| >= 3
    [[nodiscard]] int lastDelta() const;
};
```

**Responsabilidad:** Trackear el movimiento anterior para aplicar reglas de inercia.

### 3.4 `class NoteGenerator`

```cpp
class NoteGenerator {
public:
    NoteGenerator(const FretboardValidator& validator);
    [[nodiscard]] std::vector<std::unique_ptr<Note>> generateTablature();

private:
    [[nodiscard]] std::unique_ptr<Note> generateFirstNote();
    [[nodiscard]] std::unique_ptr<Note> generateNextNote(
        const Note& previous, int consecutive_same_string,
        bool must_change_string,
        const std::vector<std::unique_ptr<Note>>& previous_notes,
        MovementHistory& history
    );
    [[nodiscard]] std::vector<NoteCandidate> buildCandidates(...);
    [[nodiscard]] int calculateBaseWeight(int fret_distance) const;
    [[nodiscard]] int applyInertiaWeight(int base_weight, int fret_delta,
                                          const MovementHistory& history) const;
    [[nodiscard]] bool isValidForWindow(...) const;

    const FretboardValidator& validator_;
    RandomEngine rng_;
    std::vector<Note> valid_notes_cache_;
};
```

**Responsabilidad:** Generar notas con pesos orgánicos + inercia biomecánica.

### 3.5 `class FretboardValidator`

```cpp
class FretboardValidator {
public:
    explicit FretboardValidator(const Music::ScaleManager& scale_mgr,
                                InstrumentType instrument);

    [[nodiscard]] const InstrumentConfig& getInstrument() const noexcept;
    [[nodiscard]] bool isNoteInScale(const Note& note) const;
    [[nodiscard]] bool validateSlidingWindow(...) const;
    [[nodiscard]] std::vector<Note> getAllValidNotes() const;

private:
    const Music::ScaleManager& scale_mgr_;
    InstrumentConfig instrument_;
};
```

**Responsabilidad:** Validar notas según escala e instrumento (4 o 6 cuerdas).

### 3.6 `class TablatureGenerator` (Coordinador RAII)

```cpp
class TablatureGenerator {
public:
    explicit TablatureGenerator(InstrumentType instrument);
    void generate();
    [[nodiscard]] const std::vector<std::unique_ptr<Note>>& getNotes() const;
    [[nodiscard]] InstrumentType getInstrumentType() const;

    TablatureGenerator(const TablatureGenerator&) = delete;
    TablatureGenerator& operator=(const TablatureGenerator&) = delete;

private:
    InstrumentType instrument_;
    Music::ScaleManager scale_mgr_;
    std::unique_ptr<FretboardValidator> validator_;
    std::unique_ptr<NoteGenerator> note_gen_;
    std::vector<std::unique_ptr<Note>> notes_;
};
```

---

## 4. Sistema de Pesos Probabilísticos

### 4.1 Pesos Base por Distancia

| Distancia (trastes) | Peso | Probabilidad aprox. | Descripción |
|---------------------|------|---------------------|-------------|
| 0-2 | 70 | ~70% | Muy cómodo - grados conjuntos |
| 3 | 20 | ~20% | Moderado - estiramiento común |
| 4 | 10 | ~10% | Sorpresa - color ocasional |

### 4.2 Modificadores de Inercia

| Condición | Modificador | Efecto |
|-----------|-------------|--------|
| Mismo cuerda | +30% | Promueve fluidez |
| Post-salto grande, movimiento pequeño | +40 | Asentar la mano |
| Post-salto grande, zigzag agresivo | -50 | Evitar quiebra-muñecas |
| Salto grande consecutivo misma dirección | -10 | Prevenir fatiga |

### 4.3 Ejemplo de Cálculo

```
SITUACIÓN:
- Nota N-2: cuerda 3, traste 7
- Nota N-1: cuerda 3, traste 10 (delta = +3, salto grande)
- Candidato N: cuerda 3, traste 11 (delta = +1)

CÁLCULO:
1. Peso base por distancia (1 traste) = 70
2. Bonus misma cuerda = 70 * 1.3 = 91
3. Inercia: post-salto grande + movimiento pequeño = 91 + 40 = 131

RESULTADO: Peso final = 131 (muy favorecido)
```

---

## 5. Flujo de Generación de Datos

```
1. main() pregunta instrumento (1=Guitarra, 2=Bajo)
   │
   ▼
2. TablatureGenerator(instrument)
   │
   ├─► ScaleManager::selectRandomKeyAndScale()
   │   └─► Elige tonalidad (0-11) y escala aleatoria
   │
   ├─► FretboardValidator(scale_mgr, instrument)
   │   └─► Configura 4 o 6 cuerdas con afinación correspondiente
   │
   └─► NoteGenerator(validator)::generateTablature()
       │
       ├─► generateFirstNote()  [posición ergonómica]
       │
       └─► FOR 15 notas:
           ├─► buildCandidates() con pesos + inercia
           ├─► isValidForWindow() [3 notas, max 5 trastes]
           ├─► applyInertiaWeight() [evita zigzag]
           └─► selectWeighted() [std::discrete_distribution]

3. Formatter::printTablature(notes, num_strings)
   └─► Imprime 4 líneas (bajo) o 6 líneas (guitarra)

4. Formatter::printHarmonicInfo()
   └─► "A# Mohammedan (A# C D D# F G A A#)"

5. EasterEgg::generateAbsurdFact()
   └─► "Sabia usted que [Sujeto] [Accion] [Motivo]?"
```

---

## 6. Formato de Salida Esperado

### Guitarra (6 cuerdas)
```
=== Crazy Fingers - Tablature Generator ===
Seleccione instrumento: [1] Guitarra (6 cuerdas)  [2] Bajo (4 cuerdas): 1
[Electric Guitar - 6 strings, Standard Tuning (E2-A2-D3-G3-B3-E4)]

e|---5---7---9---12---10---8---11---14---12---10---13---15---13---11---14---17---|
B|---------------------------------------------------------------------------|
G|---------------------------------------------------------------------------|
D|---------------------------------------------------------------------------|
A|---------------------------------------------------------------------------|
E|---------------------------------------------------------------------------|

A# Mohammedan (A# C D D# F G A A#)
Sabia usted que un vikingo vegano afino su guitarra usando telequinesis en una crisis de mediana edad?
```

### Bajo (4 cuerdas)
```
=== Crazy Fingers - Tablature Generator ===
Seleccione instrumento: [1] Guitarra (6 cuerdas)  [2] Bajo (4 cuerdas): 2
[Bass Guitar - 4 strings, Standard Tuning (E1-A1-D2-G2)]

G|---5---7---9---12---10---8---11---14---12---10---13---15---13---11---14---17---|
D|---------------------------------------------------------------------------|
A|---------------------------------------------------------------------------|
E|---------------------------------------------------------------------------|

A# Mohammedan (A# C D D# F G A A#)
Sabia usted que un banquero de Wall Street fundo una banda de death metal mientras escapaba de la policia?
```

---

## 7. Archivos del Proyecto

| Archivo | Líneas | Propósito |
|---------|--------|-----------|
| `main.cpp` | ~50 | Punto de entrada, selección de instrumento |
| `music_theory.h/cpp` | ~78 / ~105 | Motor musical, traductor de notas |
| `scale_dictionary.h/cpp` | ~38 / ~170 | Diccionario de 70+ escalas |
| `fretboard.h/cpp` | ~130 / ~60 | Validación dinámica (4 o 6 cuerdas) |
| `generator.h/cpp` | ~140 / ~320 | Generador orgánico con inercia |
| `formatter.h/cpp` | ~30 / ~60 | Salida ASCII adaptable |
| `easter_egg.h/cpp` | ~150 / ~30 | 125,000 curiosidades absurdas |
| `blueprint.md` | ~300 | Este documento |

**Total líneas de código:** ~1,100 líneas distribuidas en 8 módulos

---

## 8. Consideraciones de C++20

- **`std::span`** para pasar arrays sin copias innecesarias
- **`std::array`** para configuraciones de cuerdas (tamaño fijo compile-time)
- **`std::mt19937`** con `std::random_device` para aleatoriedad de calidad
- **`std::discrete_distribution`** para selección ponderada orgánica
- **`std::optional`** para movement history (puede estar vacío al inicio)
- **`constexpr`** en todas las funciones puras de transformación de datos
- **`[[nodiscard]]`** en funciones que retornan valores críticos
- **`enum class`** para type-safe instrument selection

---

**Blueprint actualizado. Proyecto compatible con Guitarra y Bajo.**
