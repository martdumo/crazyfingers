/**
 * Music Theory Engine for CrazyFingers
 * ES6 Module - Handles scales, intervals, and pitch validation
 */

// ============================================================================
// Constants
// ============================================================================

export const SEMITONES_IN_OCTAVE = 12;
export const NUM_KEYS = 12;

// Note names (0=C, 1=C#, 2=D, ..., 11=B)
export const NOTE_NAMES = [
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
];

// ============================================================================
// Scale Dictionary - Over 70 scales from ethnomusicology consensus
// ============================================================================

export const SCALE_DICTIONARY = {
    // Common/Modes
    "Major": [2, 2, 1, 2, 2, 2, 1],
    "Harmonic Minor": [2, 1, 2, 2, 1, 3, 1],
    "Melodic Minor": [2, 1, 2, 2, 2, 2, 1],
    "Natural Minor": [2, 1, 2, 2, 1, 2, 2],
    "Pentatonic Major": [2, 2, 3, 2, 3],
    "Pentatonic Minor": [3, 2, 2, 3, 2],
    "Pentatonic Blues": [3, 2, 1, 1, 3, 2],
    "Pentatonic Neutral": [2, 3, 2, 3, 2],
    "Ionian": [2, 2, 1, 2, 2, 2, 1],
    "Dorian": [2, 1, 2, 2, 2, 1, 2],
    "Phrygian": [1, 2, 2, 2, 1, 2, 2],
    "Lydian": [2, 2, 2, 1, 2, 2, 1],
    "Mixolydian": [2, 2, 1, 2, 2, 1, 2],
    "Aeolian": [2, 1, 2, 2, 1, 2, 2],
    "Locrian": [1, 2, 2, 1, 2, 2, 2],

    // Symmetric/Altered
    "Chromatic": [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    "Whole Tone": [2, 2, 2, 2, 2, 2],
    "Octatonic (H-W)": [1, 2, 1, 2, 1, 2, 1, 2],
    "Octatonic (W-H)": [2, 1, 2, 1, 2, 1, 2, 1],
    "Augmented": [3, 1, 3, 1, 3, 1],
    "Altered": [1, 1, 2, 2, 2, 2, 2],
    "Diatonic": [2, 2, 1, 2, 2, 2, 1],
    "Diminished": [2, 1, 2, 1, 2, 1, 2, 1],
    "Diminished Half": [1, 2, 1, 2, 1, 2, 1, 2],
    "Diminished Whole": [2, 1, 2, 1, 2, 1, 2, 1],
    "Diminished Whole Tone": [1, 1, 1, 2, 2, 2, 3],
    "Dominant 7th": [5, 2, 3, 2],
    "Lydian Augmented": [2, 2, 2, 2, 1, 2, 1],
    "Lydian Minor": [2, 2, 1, 1, 2, 2, 2],
    "Lydian Diminished": [2, 2, 1, 1, 2, 2, 2],
    "Half Diminished": [1, 2, 2, 1, 2, 2, 2],

    // Jazz/Bebop
    "Bebop Major": [2, 2, 1, 2, 1, 1, 2, 2],
    "Bebop Minor": [2, 1, 2, 2, 1, 1, 2, 2],
    "Bebop Dominant": [2, 2, 1, 2, 2, 1, 1, 2],
    "Bebop Half Diminished": [1, 2, 2, 1, 1, 2, 2, 2],
    "Blues": [3, 2, 1, 1, 3, 2],
    "Major Blues Scale": [2, 1, 1, 2, 3, 2],
    "Dominant Pentatonic": [2, 2, 3, 2, 3],
    "Mixo-Blues": [2, 2, 1, 2, 2, 3],

    // Exotic & World (A-J)
    "Algerian": [2, 1, 3, 1, 1, 3, 1],
    "Arabian #1": [2, 2, 1, 1, 2, 2, 2],
    "Arabian #2": [1, 3, 1, 2, 1, 3, 1],
    "Balinese": [1, 4, 1, 4, 2],
    "Byzantine": [1, 3, 1, 2, 1, 3, 1],
    "Chinese": [4, 2, 1, 4, 1],
    "Chinese Mongolian": [2, 3, 2, 3, 2],
    "Egyptian": [2, 3, 2, 3, 2],
    "Eight Tone Spanish": [1, 2, 1, 2, 1, 2, 1, 2],
    "Ethiopian (A raray)": [1, 2, 2, 2, 1, 2, 2],
    "Ethiopian (Geez&Ezel)": [2, 1, 2, 2, 1, 2, 2],
    "Hawaiian": [2, 3, 2, 3, 2],
    "Hindu": [2, 2, 1, 2, 1, 2, 2],
    "Hindustan": [2, 2, 1, 2, 2, 1, 2],
    "Hirajoshi": [3, 1, 4, 1, 3],
    "Hungarian Major": [3, 1, 1, 3, 1, 1, 2],
    "Hungarian Gypsy": [2, 1, 3, 1, 1, 3, 1],
    "Hungarian Minor": [2, 1, 3, 1, 1, 3, 1],
    "Japanese #1": [1, 4, 2, 1, 4],
    "Japanese #2": [2, 3, 2, 3, 2],
    "Javaneese": [2, 2, 3, 2, 3],
    "Jewish (Adonai Malakh)": [2, 2, 1, 2, 2, 1, 2],
    "Jewish (Ahaba Rabba)": [1, 3, 1, 2, 1, 2, 2],

    // Exotic & World (K-Z)
    "Kumoi": [2, 1, 4, 2, 3],
    "Mohammedan": [2, 2, 1, 2, 2, 2, 1],
    "Neopolitan": [1, 2, 2, 2, 2, 2, 1],
    "Neopolitan Major": [1, 2, 2, 2, 2, 2, 1],
    "Neopolitan Minor": [1, 2, 2, 2, 1, 3, 1],
    "Oriental #1": [1, 3, 1, 1, 1, 3, 2],
    "Oriental #2": [2, 1, 3, 1, 1, 2, 2],
    "Pelog": [1, 2, 4, 1, 4],
    "Persian": [1, 3, 1, 1, 1, 3, 2],
    "Prometheus": [2, 2, 2, 3, 1, 2],
    "Prometheus Neopolitan": [2, 2, 2, 3, 1, 2],
    "Roumanian Minor": [2, 1, 3, 1, 1, 3, 1],
    "Spanish Gypsy": [1, 3, 1, 2, 1, 2, 2],
    "Super Locrian": [1, 1, 2, 2, 2, 2, 2],
    "Iwato": [1, 4, 1, 4, 2],
    "Moorish Phrygian": [1, 3, 1, 2, 1, 2, 2],
    "Double Harmonic": [1, 3, 1, 2, 1, 3, 1],
    "Enigmatic": [1, 3, 2, 2, 2, 1, 1]
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * Convert pitch class to note name
 * @param {number} pc - Pitch class (0-11)
 * @returns {string} Note name
 */
export function pitchClassToName(pc) {
    if (pc >= 0 && pc < NUM_KEYS) {
        return NOTE_NAMES[pc];
    }
    return "?";
}

/**
 * Compute scale notes from root key and intervals
 * @param {number} root - Root key index (0-11)
 * @param {number[]} intervals - Interval array
 * @returns {string} Formatted scale notes (e.g., "C D E F G A B")
 */
export function computeScaleNotes(root, intervals) {
    let current = root;
    const notes = [NOTE_NAMES[current]];

    for (const interval of intervals) {
        current = (current + interval) % SEMITONES_IN_OCTAVE;
        notes.push(NOTE_NAMES[current]);
    }

    return notes.join(" ");
}

/**
 * Parse key name string to index (case-insensitive)
 * @param {string} name - Key name (e.g., "C", "C#", "D")
 * @returns {number} Key index (0-11) or -1 if invalid
 */
export function parseKeyName(name) {
    if (!name) return -1;
    const upperName = name.toUpperCase();
    const index = NOTE_NAMES.indexOf(upperName);
    return index;
}

/**
 * Get all available scale names
 * @returns {string[]} Array of scale names
 */
export function getAllScaleNames() {
    return Object.keys(SCALE_DICTIONARY);
}

/**
 * Get intervals for a specific scale
 * @param {string} scaleName - Scale name
 * @returns {number[]} Interval array or Major intervals if not found
 */
export function getIntervals(scaleName) {
    return SCALE_DICTIONARY[scaleName] || SCALE_DICTIONARY["Major"];
}

/**
 * Get a random scale name
 * @returns {string} Random scale name
 */
export function getRandomScaleName() {
    const scaleNames = getAllScaleNames();
    const randomIndex = Math.floor(Math.random() * scaleNames.length);
    return scaleNames[randomIndex];
}

// ============================================================================
// ScaleManager Class
// ============================================================================

export class ScaleManager {
    constructor() {
        this.currentKey = 0;
        this.currentScaleName = "Major";
        this.currentIntervals = [2, 2, 1, 2, 2, 2, 1];
        this.validPitchClasses = [];
        this.scaleNotes = "";

        this.selectRandomKeyAndScale();
    }

    /**
     * Select random key and scale
     */
    selectRandomKeyAndScale() {
        this.currentKey = Math.floor(Math.random() * NUM_KEYS);
        this.currentScaleName = getRandomScaleName();
        this.currentIntervals = getIntervals(this.currentScaleName);

        this.computeValidPitchClasses();
        this.computeScaleNotes();
    }

    /**
     * Set specific key and scale
     * @param {number} key - Key index (0-11)
     * @param {string} scaleName - Scale name
     */
    setKeyAndScale(key, scaleName) {
        this.currentKey = key;
        this.currentScaleName = scaleName;
        this.currentIntervals = getIntervals(scaleName);

        this.computeValidPitchClasses();
        this.computeScaleNotes();
    }

    /**
     * Compute valid pitch classes for current scale
     */
    computeValidPitchClasses() {
        const pitchClasses = new Set();
        let current = this.currentKey;
        pitchClasses.add(current);

        for (const interval of this.currentIntervals) {
            current = (current + interval) % SEMITONES_IN_OCTAVE;
            pitchClasses.add(current);
        }

        this.validPitchClasses = Array.from(pitchClasses).sort((a, b) => a - b);
    }

    /**
     * Compute scale notes string
     */
    computeScaleNotes() {
        this.scaleNotes = computeScaleNotes(this.currentKey, this.currentIntervals);
    }

    /**
     * Get current key name
     * @returns {string} Key name
     */
    getCurrentKeyName() {
        return NOTE_NAMES[this.currentKey];
    }

    /**
     * Get current key index
     * @returns {number} Key index (0-11)
     */
    getCurrentKeyIndex() {
        return this.currentKey;
    }

    /**
     * Get current scale name
     * @returns {string} Scale name
     */
    getCurrentScaleName() {
        return this.currentScaleName;
    }

    /**
     * Get scale notes as formatted string
     * @returns {string} Scale notes
     */
    getScaleNotes() {
        return this.scaleNotes;
    }

    /**
     * Check if a pitch class belongs to current scale
     * @param {number} pitchClass - Pitch class (0-11)
     * @returns {boolean} True if in scale
     */
    isPitchInScale(pitchClass) {
        return this.validPitchClasses.includes(pitchClass);
    }

    /**
     * Check if a MIDI pitch is valid for current scale
     * @param {number} midiPitch - MIDI pitch
     * @returns {boolean} True if in scale
     */
    isMidiPitchValid(midiPitch) {
        const pitchClass = midiPitch % SEMITONES_IN_OCTAVE;
        return this.isPitchInScale(pitchClass);
    }

    /**
     * Get all valid pitch classes
     * @returns {number[]} Array of valid pitch classes
     */
    getValidPitchClasses() {
        return this.validPitchClasses;
    }
}
