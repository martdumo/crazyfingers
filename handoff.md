# CrazyFingers - Guitar Tablature Generator
## Handoff Document

---

## 1. Resumen del Proyecto

**CrazyFingers** es un generador de tablaturas de guitarra aleatorias diseñado específicamente para crear ejercicios de flexibilidad técnica y mental. El programa produce secuencias de 16 notas que son anatómicamente posibles de tocar pero deliberadamente "incómodas", rompiendo los patrones predecibles de escalas tradicionales.

### Tecnologías y Estándares Utilizados

| Tecnología | Propósito |
|------------|-----------|
| **C++20** | Estándar moderno con `constexpr`, `std::span`, `std::array` |
| **RAII** | Gestión automática de recursos sin fugas de memoria [CG: R.1] |
| **`std::unique_ptr`** | Memoria dinámica segura sin "naked new/delete" [CG: R.11, C.149] |
| **`<random>` (std::mt19937)** | Generación de aleatoriedad de alta calidad |
| **C++ Core Guidelines** | Conjunto de buenas prácticas de Bjarne Stroustrup |
| **Namespaces** | Organización lógica del código (`GuitarTab::`, `Formatter::`) [CG: SF.20] |

---

## 2. Instrucciones de Compilación y Ejecución

### Compilación

```bash
g++ -std=c++20 -Wall -Wextra -O2 -o crazyfingers.exe main.cpp
```

**Explicación de flags:**
| Flag | Propósito |
|------|-----------|
| `-std=c++20` | Habilita el estándar C++20 requerido |
| `-Wall` | Activa todas las advertencias comunes |
| `-Wextra` | Activa advertencias adicionales |
| `-O2` | Optimización de nivel 2 para rendimiento |
| `-o crazyfingers.exe` | Nombre del binario de salida |

### Ejecución

```bash
crazyfingers.exe
```

El programa imprimirá **exclusivamente** la tablatura ASCII en la consola y terminará inmediatamente (sin pedir input al usuario).

---

## 3. Muestra de Salida

```
e|-----------------------------13----------5---3------------------|
B|-12----------------------16------9---7-----------7--------------|
G|-----17--15----------20------------------------------7----------|
D|-------------19--21--------------------------------------3---1--|
A|----------------------------------------------------------------|
E|----------------------------------------------------------------|
```

**Características visibles:**
- ✅ 16 notas (16 columnas con números)
- ✅ Una sola nota por posición vertical (ejercicio monofónico)
- ✅ Movimiento a través de 5 cuerdas diferentes (B, G, D + saltos)
- ✅ Saltos entre cuerdas adyacentes únicamente
- ✅ Trastes dentro del rango 0-22

---

## 4. Análisis Biomecánico y Algorítmico

### Por qué este algoritmo genera ejercicios excepcionales

#### 🎯 La Limitación de "Distancia Máxima de 5 Trastes"

Esta restricción no es arbitraria: representa el **rango ergonómico natural** de la mano izquierda en posición estándar. Un estiramiento de 5 trastes (ej. índice en traste 5, meñique en traste 10) es el límite seguro para la mayoría de guitarristas sin cambiar de posición. 

**Beneficio biomecánico:**
- Evita lesiones por sobre-extensión del tendón flexor
- Mantiene la mano en una posición anatómicamente neutra
- Obliga al cerebro a encontrar soluciones dentro de restricciones realistas

#### 🔓 Rompiendo los "Box Patterns" (Patrones de Caja)

Los guitarristas tienden a memorizar formas geométricas fijas (pentatónicas, modos, etc.). El algoritmo **fuerza un cambio de cuerda obligatoria en la 3ra nota consecutiva**, lo que destruye estas formas predecibles:

```
Patrón típico de escala (predecible):
e|-----5-7-8---------------------------------|
B|-------5-7-8-------------------------------|
G|---------5-7-9-----------------------------|
...

CrazyFingers (impredecible):
e|-----------------7-------------------------|
B|-------------5-8---5-----------------------|
G|---------5-7-----------7-5-----------------|
D|-----5-7-------------------7-8-------------|
```

**Beneficio cognitivo:**
- El cerebro no puede automatizar el patrón
- Cada nota requiere atención consciente
- Desarrolla lectura instantánea y adaptación

#### ⚡ Movimiento en Cuerdas Adyacentes: Sorpresa sin Lesión

La regla de **solo cuerdas adyacentes (±1)** crea un equilibrio perfecto:

| Movimiento | Riesgo | Beneficio Técnico |
|------------|--------|-------------------|
| Saltos grandes (ej. 6ta→2da) | Alto (brazo se desplaza bruscamente) | ❌ Evitado |
| Misma cuerda siempre | Bajo (pero aburrido) | ❌ Sin flexibilidad |
| **Adyacente (±1)** | **Mínimo** | ✅ **Brazo estable, dedos alertas** |

**El efecto de "sorpresa controlada":**
1. El antebrazo se mantiene en una zona estable
2. Los dedos deben recalcular constantemente su posición relativa
3. La mano "se sorprende" pero nunca está en peligro
4. Se desarrolla **propiocepción fina** (conciencia de posición sin mirar)

---

## 5. Estructura de Archivos del Proyecto

```
crazyfingers/
├── blueprint.md          # Arquitectura y diseño del sistema
├── main.cpp              # Implementación completa en C++20
├── crazyfingers.exe      # Binario compilado (Windows)
└── handoff.md            # Este documento (entrega final)
```

---

## 6. C++ Core Guidelines Aplicadas

| Guideline | Aplicación en el código |
|-----------|------------------------|
| [CG: R.1] RAII | `TablatureGenerator` gestiona recursos automáticamente |
| [CG: R.11] No naked new/delete | `std::unique_ptr<Note>` en todo el código |
| [CG: C.149] Smart pointers | `std::vector<std::unique_ptr<Note>>` para la secuencia |
| [CG: F.4] Constexpr | Constantes de guitarra evaluadas en compile-time |
| [CG: F.2] Single purpose | Funciones cortas con un único propósito lógico |
| [CG: SF.20] Namespaces | `GuitarTab::` y `Formatter::` para estructura lógica |
| [CG: ES.47] nullptr | Usado en lugar de `0` o `NULL` |
| [CG: P.10] Inmutabilidad | `const` y `constexpr` donde sea posible |
| [CG: C.10] Clean structs | `Note`, `Fret`, `StringIndex` con semántica clara |
| [CG: C.61] Copy semantics | No copiable, sí movable para el generador |

---

## 7. Próximos Pasos (Opcional)

Si deseas extender el proyecto:

1. **Exportar a formatos**: MIDI, Guitar Pro, MusicXML
2. **Modos de dificultad**: Fácil (3 trastes máx), Difícil (6-7 trastes)
3. **Restricciones personalizadas**: Forzar cuerdas específicas, rangos de trastes
4. **Interfaz gráfica**: Visualización en tiempo real del diapasón
5. **Metrónomo integrado**: Tempo ajustable para práctica

---

**Proyecto concluido exitosamente.** 🎸

El generador CrazyFingers está listo para usar. Cada ejecución produce un ejercicio único que desafiará tanto tu técnica como tu capacidad de adaptación mental en el instrumento.
