<div align="center">

# 🎸 CrazyFingers

### Generador de Tablaturas con Biomecánica Avanzada

[![C++](https://img.shields.io/badge/C%2B%2B-20-blue?style=for-the-badge&logo=cplusplus)](https://isocpp.org/)
[![JavaScript](https://img.shields.io/badge/JavaScript-ES6+-yellow?style=for-the-badge&logo=javascript)](https://developer.mozilla.org/es/docs/Web/JavaScript)
[![HTML5](https://img.shields.io/badge/HTML5-5-orange?style=for-the-badge&logo=html5)](https://html5.org/)
[![CSS3](https://img.shields.io/badge/CSS3-3-blue?style=for-the-badge&logo=css3)](https://www.w3.org/Style/CSS/)

**Hecha por Martin Alfredo Dumont**  
*Todos los Derechos Reservados © 2024*

---

</div>

## 📖 Descripción

**CrazyFingers** es un generador de ejercicios de guitarra y bajo que utiliza algoritmos biomecánicos avanzados para crear tablaturas musicales y ergonómicas. Diseñado para músicos que quieren mejorar su técnica sin lesionarse.

### 🌟 Características Principales

- **🎯 Position Box Heuristic**: Mantiene la mano en una posición fija (±4 trastes) para entrenar los dedos, no la muñeca
- **🎵 Pitch Control**: Limita los saltos melódicos a 1 octava (ventana local) y 2 octavas (rango global)
- **🎸 Soporte Multi-Instrumento**: Guitarra (6 cuerdas) y Bajo (4 cuerdas)
- **🌍 70+ Escalas**: Desde escalas comunes hasta modos exóticos de todo el mundo
- **🎲 Generación Aleatoria**: Cada ejercicio es único pero siempre tocable
- **💻 Doble Versión**: CLI nativa en C++ y Web App en JavaScript

---

## 🖼️ Capturas

<div align="center">

**Versión Web - Argentinian Night Theme**  
*Interfaz oscura con acentos celestes y dorados inspirados en la bandera argentina*

</div>

---

## 🚀 Instalación y Uso

### Versión C++ (CLI)

#### Requisitos
- Compilador C++20 (GCC, Clang, MSVC)
- Sistema: Windows, Linux o macOS

#### Compilación
```bash
cd F:\Proyectos\crazyfingers
g++ -std=c++20 -Wall -Wextra -o crazyfingers.exe \
    main.cpp generator.cpp random_engine.cpp \
    fretboard.cpp music_theory.cpp scale_dictionary.cpp \
    formatter.cpp easter_egg.cpp
```

#### Ejecución
```bash
./crazyfingers.exe
```

#### Menú Interactivo
```
=== CRAZY FINGERS - MAIN MENU ===
[1] Generar Guitarra (Aleatorio)
[2] Generar Bajo (Aleatorio)
[3] Salir
[4] Opciones Avanzadas: Guitarra
[5] Opciones Avanzadas: Bajo
=================================
```

---

### Versión Web

#### Requisitos
- Navegador moderno (Chrome, Firefox, Edge, Safari)
- Servidor HTTP local (para ES6 modules)

#### Ejecución
```bash
cd web_version
python -m http.server 8000
```

Luego abre: `http://localhost:8000`

#### Estructura de Archivos Web
```
web_version/
├── index.html          # Página principal
├── styles.css          # Tema Argentinian Night
├── app.js              # Entry point de la aplicación
├── generator.js        # Lógica de generación (Position Box + Pitch Control)
├── musicTheory.js      # Teoría musical (70+ escalas)
└── easterEgg.js        # Frases absurdas aleatorias
```

---

## 🎼 Algoritmo Biomecánico

### Position Box (Cajón de Posición)
```
anchor_fret = traste de la primera nota
min_fret = max(0, anchor_fret - 4)
max_fret = min(22, anchor_fret + 4)

→ Todas las 16 notas deben estar dentro de [min_fret, max_fret]
```

### Pitch Control (Control de Altura)

| Regla | Límite | Descripción |
|-------|--------|-------------|
| **Local Range** | 12 semitonos | Ventana de 5 notas consecutivas ≤ 1 octava |
| **Global Range** | 24 semitonos | Todo el ejercicio ≤ 2 octavas |

### Weight System (Sistema de Pesos)

| Distancia | Peso | Probabilidad |
|-----------|------|--------------|
| 0-2 frets | 60 | Alta (cómodo) |
| 3 frets | 30 | Media (moderado) |
| 4 frets | 10 | Baja (estiramiento) |

---

## 🎸 Escalas Disponibles

### Comunes / Modos
Major, Natural Minor, Harmonic Minor, Melodic Minor, Pentatonic Major, Pentatonic Minor, Pentatonic Blues, Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian

### Simétricas / Alteradas
Chromatic, Whole Tone, Octatonic (H-W), Octatonic (W-H), Augmented, Altered, Diminished, Dominant 7th, Lydian Augmented, Half Diminished

### Jazz / Bebop
Bebop Major, Bebop Minor, Bebop Dominant, Bebop Half Diminished, Blues, Major Blues Scale, Dominant Pentatonic

### Exóticas y del Mundo
Spanish Gypsy, Hungarian Minor, Japanese #1, Hirajoshi, Algerian, Balinese, Byzantine, Chinese, Egyptian, Hawaiian, Hindu, Neopolitan, Persian, Prometheus, y muchas más...

**Total: 70+ escalas** de etnomusicología consensuada.

---

## 📁 Estructura del Proyecto

```
crazyfingers/
├── main.cpp                  # Entry point CLI
├── generator.h / .cpp        # Generador de tablaturas
├── random_engine.h / .cpp    # Motor aleatorio con pesos
├── fretboard.h / .cpp        # Validador del diapasón
├── music_theory.h / .cpp     # Gestor de escalas
├── scale_dictionary.h / .cpp # Diccionario de 70+ escalas
├── formatter.h / .cpp        # Formateo ASCII de tablaturas
├── easter_egg.h / .cpp       # Frases absurdas (50×50×50)
├── crazyfingers.exe          # Binario compilado
│
└── web_version/              # Versión Web
    ├── index.html            # Página principal
    ├── styles.css            # Tema Argentinian Night
    ├── app.js                # Entry point
    ├── generator.js          # Generador (mismo algoritmo)
    ├── musicTheory.js        # Teoría musical
    └── easterEgg.js          # Frases absurdas
```

---

## 🛠️ Tecnologías Utilizadas

| Tecnología | Versión | Propósito |
|------------|---------|-----------|
| ![C++](https://img.shields.io/badge/C++-20-blue?style=flat) | C++20 | Versión CLI nativa |
| ![JavaScript](https://img.shields.io/badge/JavaScript-ES6+-yellow?style=flat) | ES6+ | Versión Web |
| ![HTML5](https://img.shields.io/badge/HTML5-5-orange?style=flat) | HTML5 | Estructura Web |
| ![CSS3](https://img.shields.io/badge/CSS3-Custom-blue?style=flat) | CSS3 | Tema Argentinian Night |

---

## 📄 Licencia y Derechos de Autor

<div align="center">

### **© 2024 Martin Alfredo Dumont - Todos los Derechos Reservados**

</div>

Este software es propiedad intelectual de **Martin Alfredo Dumont**. Queda prohibida su reproducción, distribución, modificación o uso comercial sin autorización expresa del autor.

#### Términos de Uso

1. **Uso Personal**: Permitido para práctica musical personal
2. **Uso Educativo**: Permitido en instituciones educativas con atribución
3. **Uso Comercial**: Requiere licencia explícita del autor
4. **Modificaciones**: No se permite crear obras derivadas sin autorización
5. **Distribución**: No se permite redistribuir el código fuente o binarios

#### Contacto para Licencias

Para consultas sobre licencias comerciales o colaboraciones:
- **Autor**: Martin Alfredo Dumont
- **Proyecto**: CrazyFingers

---

## 🙏 Agradecimientos

- A todos los músicos que probaron las versiones alpha
- A la comunidad de etnomusicología por las escalas del mundo
- Al diseño "Tokyo Night" que inspiró el tema Argentinian Night

---

## 🎵 Ejemplo de Salida

```
[Electric Guitar - 6 strings, Standard Tuning (E2-A2-D3-G3-B3-E4)]

e|-----10------10-------------------------------------------------|
B|---------------------11--------------13------------------11-----|
G|---------8-----------------------------------11------13---------|
D|-12--------------12------12--14--10------13------------------12-|
A|-------------------------------------------------15-------------|
E|----------------------------------------------------------------|

D# Neopolitan Major (D# E F# G# A# C D D#)

🎸 Sabía usted que el mismisimo diablo toco naked en publico mientras viajaba en el tiempo?
```

---

<div align="center">

### 🇦🇷 Hecho en Argentina con 💙 y 🎸

**Martin Alfredo Dumont** © 2024

</div>
