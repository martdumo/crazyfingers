# CrazyFingers - Guitar Tablature Generator
## Handoff Document (v2.0 - Music Theory Engine)

---

## 1. Resumen del Proyecto

**CrazyFingers** es un generador profesional de tablaturas de guitarra que combina teoría musical con restricciones biomecánicas para crear ejercicios de flexibilidad técnica y mental. A diferencia de la versión anterior, este sistema primero selecciona una **tonalidad y escala musical** y SOLO usa notas pertenecientes a esa escala, garantizando coherencia armónica.

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

El proyecto está estructurado en **5 módulos independientes** (ninguno supera 200 líneas):

```
┌─────────────────────────────────────────────────────────────────┐
│                         main.cpp                                │
│                      (Entry Point ~30 líneas)                   │
└─────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│                    generator.h/cpp                              │
│         (Coordinador + Generador de Notas ~180 líneas)          │
└─────────────────────────────────────────────────────────────────┘
                                │
                ┌───────────────┼───────────────┐
                ▼               ▼               ▼
┌─────────────────────┐ ┌─────────────┐ ┌─────────────────────┐
│  music_theory.h/cpp │ │fretboard.h/ │ │ formatter.h/cpp     │
│  (Motor Musical)    │ │   cpp       │ │ (Salida ASCII)      │
│  ~100 líneas        │ │ ~60 líneas  │ │ ~50 líneas          │
└─────────────────────┘ └─────────────┘ └─────────────────────┘
```

### Archivos del Proyecto

| Archivo | Líneas | Responsabilidad |
|---------|--------|-----------------|
| `main.cpp` | ~30 | Punto de entrada, orquestación |
| `music_theory.h/cpp` | ~85 / ~100 | Motor de teoría musical (17 escalas, 12 tonalidades) |
| `fretboard.h/cpp` | ~90 / ~60 | Validación de notas, cálculo de pitches MIDI |
| `generator.h/cpp` | ~85 / ~180 | Generación aleatoria con restricciones biomecánicas |
| `formatter.h/cpp` | ~35 / ~50 | Formateo y salida ASCII a consola |

---

## 3. Instrucciones de Compilación y Ejecución

### Compilación

```bash
g++ -std=c++20 -Wall -Wextra -O2 -o crazyfingers.exe main.cpp music_theory.cpp fretboard.cpp generator.cpp formatter.cpp
```

**Explicación de flags:**
| Flag | Propósito |
|------|-----------|
| `-std=c++20` | Estándar C++20 requerido |
| `-Wall` | Todas las advertencias comunes |
| `-Wextra` | Advertencias adicionales |
| `-O2` | Optimización de nivel 2 |
| `-o crazyfingers.exe` | Nombre del binario |

### Ejecución

```bash
crazyfingers.exe
```

El programa imprime **exclusivamente** la tablatura ASCII y la inspiración armónica, luego termina (sin input del usuario).

---

## 4. Muestra de Salida

```
e|-------------11--------------15--12-----------------------------|
B|---------11------12------12----------12------12-----------------|
G|-9---8---------------12------------------15------15-------------|
D|-----------------------------------------------------17------21-|
A|---------------------------------------------------------19-----|
E|----------------------------------------------------------------|

Inspiración armónica: Key de G - Hirajoshi
```

---

## 5. Motor de Teoría Musical

### Tonalidades Disponibles (12)

| Índice | Tonalidad | Índice | Tonalidad |
|--------|-----------|--------|-----------|
| 0 | C | 6 | F# |
| 1 | C# | 7 | G |
| 2 | D | 8 | G# |
| 3 | D# | 9 | A |
| 4 | E | 10 | A# |
| 5 | F | 11 | B |

### Escalas Implementadas (17)

| Categoría | Escalas |
|-----------|---------|
| **Básicas** | Mayor, Menor Natural |
| **Pentatónicas** | Pentatónica Mayor, Pentatónica Menor, Blues |
| **Modos Griegos** | Dorian, Phrygian, Lydian, Mixolydian, Locrian |
| **Jazz/Bebop** | Dominant Bebop, Major Bebop |
| **Menores** | Harmónica, Melódica |
| **Exóticas** | Arabic (Double Harmonic), Hirajoshi, Hungarian Minor |

### Cálculo de Notas Válidas

```
pitch = base_cuerda + traste
pitch_class = pitch % 12

Una nota es VÁLIDA si pitch_class ∈ notas_de_la_escala
```

**Afinaciones base (MIDI):**
| Cuerda | Nota | MIDI |
|--------|------|------|
| 1ra (aguda) | E4 | 64 |
| 2da | B3 | 59 |
| 3ra | G3 | 55 |
| 4ta | D3 | 50 |
| 5ta | A2 | 45 |
| 6ta (grave) | E2 | 40 |

---

## 6. Reglas Biomecánicas

| Regla | Valor | Propósito |
|-------|-------|-----------|
| **Máx notas consecutivas misma cuerda** | 3 | Obliga a moverse por el mástil |
| **Distancia máx entre notas consecutivas** | 3 trastes | Movimiento de pivote, no saltos bruscos |
| **Ventana deslizante (3 notas)** | Máx 5 trastes | Alcance real de los 4 dedos |
| **Cuerdas adyacentes** | ±1 | Movimiento ergonómico del brazo |

### Ejemplo de Ventana Deslizante

```
Notas: D7 → A8 → D9
Trastes: 7, 8, 9
Rango: 9 - 7 = 2 ✓ (≤ 5, válido)

Notas: D4 → A12 → D6  ← INVÁLIDO
Trastes: 4, 12, 6
Rango: 12 - 4 = 8 ✗ (> 5, inválido)
```

---

## 7. C++ Core Guidelines Aplicadas

| Guideline | Aplicación |
|-----------|------------|
| [CG: R.1] RAII | `TablatureGenerator` gestiona recursos automáticamente |
| [CG: R.11] No naked new/delete | `std::unique_ptr<Note>` en todo el código |
| [CG: C.149] Smart pointers | `std::vector<std::unique_ptr<Note>>` |
| [CG: F.4] Constexpr | Constantes compiladas (`NUM_STRINGS`, `MAX_FRET`, etc.) |
| [CG: F.2] Single purpose | Funciones cortas con un único propósito |
| [CG: SF.20] Namespaces | `Music::`, `Guitar::`, `Formatter::` |
| [CG: SF.3] Headers for interfaces | `.h` para declaraciones, `.cpp` para implementación |
| [CG: SF.5] Implementation in .cpp | Lógica en archivos `.cpp` |
| [CG: C.61] Copy semantics | No copiable, sí movable |

---

## 8. Flujo de Generación

```
1. ScaleManager::selectRandomKeyAndScale()
   └─► Elige tonalidad (0-11) y escala (0-16) aleatoriamente
   └─► Calcula pitch classes válidos

2. FretboardValidator::getAllValidNotes()
   └─► Escanea todo el diapasón (6 cuerdas × 23 trastes)
   └─► Filtra notas que pertenecen a la escala

3. NoteGenerator::generateTablature()
   └─► Genera 1ra nota (aleatoria dentro de la escala)
   └─► Para cada nota siguiente (2-16):
       ├─► Selecciona cuerda (adyacente o misma, máx 3 consecutivas)
       ├─► Selecciona traste (máx ±3 del anterior)
       ├─► Valida ventana deslizante (3 notas, máx 5 trastes de rango)
       └─► Valida que pertenezca a la escala

4. Formatter::printTablature()
   └─► Imprime 6 líneas (e, B, G, D, A, E)
   └─► Cada columna tiene UNA sola nota

5. Formatter::printHarmonicInfo()
   └─► Imprime "Inspiración armónica: Key de X - Escala Y"
```

---

## 9. Próximas Extensiones (Opcional)

1. **Exportar a MIDI** - Generar archivo .mid para tocar con DAW
2. **Tempo variable** - Metrónomo integrado con BPM ajustable
3. **Patrones rítmicos** - Corcheas, tresillos, semicorcheas
4. **Técnicas específicas** - Hammer-on, pull-off, slide, bend
5. **Restricciones personalizadas** - Solo cuerdas graves, solo trastes 1-5, etc.
6. **Interfaz gráfica** - Visualización del mástil en tiempo real

---

**Proyecto concluido exitosamente.** 🎸

El generador CrazyFingers v2.0 combina teoría musical profesional con ergonomía biomecánica para crear ejercicios únicos, musicalmente coherentes y anatómicamente seguros.
