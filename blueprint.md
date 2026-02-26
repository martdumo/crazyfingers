# Crazy Fingers - Guitar Tablature Generator
## Blueprint de Arquitectura C++20

---

## 1. Arquitectura del Software

### 1.1 Visión General

El sistema sigue un diseño **modular y orientado a componentes** con separación clara de responsabilidades:

```
┌─────────────────────────────────────────────────────────────┐
│                      main.cpp                               │
│                    (Entry Point)                            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                  TablatureGenerator                         │
│              (Coordinador principal - RAII)                 │
└─────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┼───────────────┐
              ▼               ▼               ▼
┌─────────────────────┐ ┌─────────────┐ ┌─────────────────────┐
│   NoteGenerator     │ │  GuitarRules│ │   TabFormatter      │
│  (Aleatoriedad)     │ │ (Validación)│ │   (Salida consola)  │
└─────────────────────┘ └─────────────┘ └─────────────────────┘
```

### 1.2 Principios de Diseño Aplicados

| Principio | Aplicación |
|-----------|------------|
| **RAII** [CG: R.1] | `TablatureGenerator` gestiona recursos en su ciclo de vida |
| **Single Responsibility** [CG: F.2] | Cada clase tiene un propósito único |
| **Constexpr** [CG: F.4] | Constantes de guitarra evaluadas en compile-time |
| **No naked new/delete** [CG: R.11] | `std::vector` y `std::unique_ptr` para memoria dinámica |
| **Type Safety** | `struct Note` en lugar de `std::pair` para semántica clara |

---

## 2. Lógica del Algoritmo Paso a Paso

### 2.1 Constantes del Dominio (Compile-Time)

```
NUM_STRINGS     = 6
NUM_FRETS       = 22  (0-22 inclusive)
NUM_NOTES       = 16
MAX_FRET_JUMP   = 5   (distancia máxima entre trastes consecutivos)
STRING_RANGE    = 1   (solo cuerdas adyacentes)
```

### 2.2 Mapeo de Cuerdas (Notación Estándar)

| Índice | Cuerda | Nota al aire |
|--------|--------|--------------|
| 0 | 1ra (aguda) | E (Mi alto) |
| 1 | 2da | B (Si) |
| 2 | 3ra | G (Sol) |
| 3 | 4ta | D (Re) |
| 4 | 5ta | A (La) |
| 5 | 6ta (grave) | E (Mi bajo) |

### 2.3 Algoritmo de Generación

```
ALGORITMO: GenerateTablature
┌─────────────────────────────────────────────────────────────────┐
│ 1. INICIALIZACIÓN                                               │
│    - Crear vector de 16 notas (std::vector<Note>)               │
│    - Inicializar generador aleatorio con seed temporal          │
│                                                                 │
│ 2. PRIMERA NOTA (aleatoria pura)                                │
│    - string_idx = random(0, 5)                                  │
│    - fret = random(0, 22)                                       │
│    - notes[0] = Note{string_idx, fret}                          │
│                                                                 │
│ 3. GENERAR 15 NOTAS RESTANTES                                   │
│    FOR i = 1 TO 15:                                             │
│                                                                 │
│      a) SELECCIÓN DE CUERDA (adyacente o misma)                 │
│         prev_string = notes[i-1].string                         │
│         delta_string = random(-1, 0, 1)                         │
│         new_string = clamp(prev_string + delta_string, 0, 5)    │
│                                                                 │
│      b) SELECCIÓN DE TRASTE (máx 5 trastes de distancia)        │
│         prev_fret = notes[i-1].fret                             │
│         delta_fret = random(-5, 5)                              │
│         new_fret = clamp(prev_fret + delta_fret, 0, 22)         │
│                                                                 │
│      c) VALIDACIÓN DE TOCABILIDAD                               │
│         - Si new_string == prev_string Y new_fret == prev_fret: │
│           → Forzar movimiento (regenerar delta_fret ≠ 0)        │
│         - Verificar que sea anatómicamente posible              │
│                                                                 │
│      d) ALMACENAR                                               │
│         notes[i] = Note{new_string, new_fret}                   │
│                                                                 │
│ 4. FORMATEO DE SALIDA                                           │
│    - Construir 6 líneas (una por cuerda)                        │
│    - Para cada posición (0-15):                                 │
│      * Colocar número de traste en la cuerda correspondiente    │
│      * Poner '-' en las demás cuerdas                           │
│    - Imprimir en orden descendente (6ta → 1ra)                  │
│                                                                 │
│ 5. TERMINACIÓN                                                  │
│    - Imprimir newline final                                     │
│    - Destructor libera recursos automáticamente (RAII)          │
└─────────────────────────────────────────────────────────────────┘
```

### 2.4 Función de Clamp (Compile-Time)

```cpp
constexpr int clamp(int value, int min_val, int max_val) {
    return (value < min_val) ? min_val : (value > max_val) ? max_val : value;
}
```

### 2.5 Restricciones de Tocabilidad

| Restricción | Valor | Justificación |
|-------------|-------|---------------|
| Salto máximo de trastes | 5 | Evita estiramientos imposibles de la mano izquierda |
| Cuerdas adyacentes | ±1 | Movimiento natural del brazo/antebrazo |
| Notas consecutivas diferentes | Sí | Evita notas repetidas (ejercicio de flexibilidad) |
| Rango de trastes | 0-22 | Diapasón estándar de guitarra eléctrica |

---

## 3. Estructura de Clases/Structs

### 3.1 `struct Note`

```cpp
struct Note {
    int string_idx;  // 0-5 (0 = 1ra cuerda aguda, 5 = 6ta cuerda grave)
    int fret;        // 0-22 (0 = al aire)
    
    // Semántica clara vs std::pair<int,int>
};
```

**Responsabilidad:** Representar una única nota en la tablatura.

### 3.2 `class NoteGenerator`

```cpp
class NoteGenerator {
public:
    NoteGenerator();
    Note generateFirstNote() const;
    Note generateNextNote(const Note& previous) const;
    
private:
    std::mt19937 rng_;
    std::uniform_int_distribution<int> string_dist_;
    std::uniform_int_distribution<int> fret_dist_;
    std::uniform_int_distribution<int> delta_string_dist_;
    std::uniform_int_distribution<int> delta_fret_dist_;
    
    static constexpr int MIN_STRING = 0;
    static constexpr int MAX_STRING = 5;
    static constexpr int MIN_FRET = 0;
    static constexpr int MAX_FRET = 22;
    static constexpr int MAX_DELTA_FRET = 5;
};
```

**Responsabilidad:** Generar notas aleatorias válidas según las reglas de guitarra.

### 3.3 `class TabFormatter`

```cpp
class TabFormatter {
public:
    static void print(const std::vector<Note>& notes);
    
private:
    static constexpr std::array<const char*, 6> STRING_LABELS = {
        "e", "B", "G", "D", "A", "E"
    };
    static constexpr int TAB_WIDTH = 4;  // "---X" por nota
};
```

**Responsabilidad:** Formatear y imprimir la tablatura en formato estándar de consola.

### 3.4 `class TablatureGenerator` (Coordinador RAII)

```cpp
class TablatureGenerator {
public:
    TablatureGenerator();
    ~TablatureGenerator() = default;
    
    void generate();
    void print() const;
    
    // Regla de oro: no copiable, sí movable [CG: C.61]
    TablatureGenerator(const TablatureGenerator&) = delete;
    TablatureGenerator& operator=(const TablatureGenerator&) = delete;
    TablatureGenerator(TablatureGenerator&&) = default;
    TablatureGenerator& operator=(TablatureGenerator&&) = default;
    
private:
    std::vector<Note> notes_;
    NoteGenerator generator_;
    
    static constexpr int NUM_NOTES = 16;
};
```

**Responsabilidad:** Coordinar la generación, almacenar el estado, gestionar recursos vía RAII.

---

## 4. Diagrama de Flujo de Datos

```
┌──────────────┐     ┌─────────────────┐     ┌──────────────┐
│ NoteGenerator│────▶│  Note (struct)  │◀────│TablatureGen. │
└──────────────┘     └─────────────────┘     └──────────────┘
                              │                      │
                              │ vector<Note>         │
                              ▼                      │
                     ┌─────────────────┐             │
                     │  TabFormatter   │◀────────────┘
                     └─────────────────┘
                              │
                              ▼
                     ┌─────────────────┐
                     │   Consola       │
                     │  e|---2---5---  │
                     │  B|---3---1---  │
                     │  ...            │
                     └─────────────────┘
```

---

## 5. Formato de Salida Esperado

```
e|---0---3---7---12---8---5---9---14---10---6---11---15---11---7---12---16---|
B|---------------------------------------------------------------------------|
G|---------------------------------------------------------------------------|
D|---------------------------------------------------------------------------|
A|---------------------------------------------------------------------------|
E|---------------------------------------------------------------------------|
```

*(Ejemplo ilustrativo - las notas reales serán aleatorias dentro de las reglas)*

---

## 6. Archivos del Proyecto

| Archivo | Propósito |
|---------|-----------|
| `main.cpp` | Punto de entrada, instancia y ejecuta `TablatureGenerator` |
| `note.h` | Definición de `struct Note` |
| `note_generator.h/cpp` | Implementación de `NoteGenerator` |
| `tab_formatter.h/cpp` | Implementación de `TabFormatter` |
| `tablature_generator.h/cpp` | Implementación del coordinador RAII |
| `guitar_constants.h` | Constantes `constexpr` del dominio |
| `blueprint.md` | Este documento |

---

## 7. Consideraciones de C++20

- **`std::span`** para pasar arrays sin copias innecesarias
- **`std::array`** para `STRING_LABELS` (tamaño fijo conocido en compile-time)
- **`std::mt19937`** con `std::random_device` para aleatoriedad de calidad
- **`constexpr`** en todas las funciones puras de transformación de datos
- **`[[nodiscard]]`** en funciones que retornan valores críticos

---

**Blueprint generado. Esperando luz verde para el código**
