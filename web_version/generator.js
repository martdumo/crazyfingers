/**
 * Tablature Generator for CrazyFingers
 * ES6 Module - Implements Position Box heuristic with Pitch Control
 */

import { ScaleManager } from './musicTheory.js';
import { generateAbsurdFact } from './easterEgg.js';

// ============================================================================
// Constants - Position Box Heuristic + Pitch Control
// ============================================================================

const NUM_NOTES = 16;
const MAX_CONSECUTIVE_SAME_STRING = 3;
const POSITION_BOX_RADIUS = 4;  // ±4 frets from anchor

// Pitch Control Constants
const MAX_LOCAL_RANGE = 12;     // 1 octave (5-note window)
const MAX_GLOBAL_RANGE = 24;    // 2 octaves (entire exercise)
const LOCAL_WINDOW_SIZE = 4;    // Check last 4 notes + candidate

// Instrument configurations
const GUITAR_NUM_STRINGS = 6;
const BASS_NUM_STRINGS = 4;
const MIN_FRET = 0;
const MAX_FRET = 22;

// Guitar: E2=40, A2=45, D3=50, G3=55, B3=59, E4=64
const GUITAR_OPEN_STRING_MIDI = [64, 59, 55, 50, 45, 40];
// Bass: E1=28, A1=33, D2=38, G2=43
const BASS_OPEN_STRING_MIDI = [43, 38, 33, 28];

// String labels
const GUITAR_STRING_LABELS = ["e", "B", "G", "D", "A", "E"];
const BASS_STRING_LABELS = ["G", "D", "A", "E"];

// Weight constants for organic movement
const WEIGHT_CLOSE = 60;       // 0-2 frets: comfortable
const WEIGHT_MEDIUM = 30;      // 3 frets: moderate
const WEIGHT_FAR = 10;         // 4 frets: stretch

// ============================================================================
// Helper: Weighted Random Selection
// ============================================================================

/**
 * Select an index based on weights using weighted random distribution
 * @param {number[]} weights - Array of weight values
 * @returns {number} Selected index
 */
function selectWeighted(weights) {
    if (weights.length === 0) return -1;
    if (weights.length === 1) return 0;

    const totalWeight = weights.reduce((sum, w) => sum + w, 0);
    let random = Math.random() * totalWeight;

    for (let i = 0; i < weights.length; i++) {
        random -= weights[i];
        if (random <= 0) {
            return i;
        }
    }

    return weights.length - 1;
}

// ============================================================================
// Helper: Get MIDI Pitch
// ============================================================================

/**
 * Get MIDI pitch for a note
 * @param {number} stringIdx - String index (0 = highest pitch)
 * @param {number} fret - Fret number
 * @param {number} numStrings - Total strings (4 for bass, 6 for guitar)
 * @returns {number} MIDI pitch
 */
function getPitch(stringIdx, fret, numStrings) {
    const openStrings = numStrings === GUITAR_NUM_STRINGS
        ? GUITAR_OPEN_STRING_MIDI
        : BASS_OPEN_STRING_MIDI;
    return openStrings[stringIdx] + fret;
}

// ============================================================================
// Position Box - Anchors the hand position for entire exercise
// ============================================================================

class PositionBox {
    constructor() {
        this.anchorFret = 0;
        this.minFret = 0;
        this.maxFret = 0;
    }

    /**
     * Initialize position box based on first note's fret
     * @param {number} firstFret - First note's fret
     */
    initialize(firstFret) {
        this.anchorFret = firstFret;
        this.minFret = Math.max(MIN_FRET, this.anchorFret - POSITION_BOX_RADIUS);
        this.maxFret = Math.min(MAX_FRET, this.anchorFret + POSITION_BOX_RADIUS);
    }

    /**
     * Check if a fret is within the position box
     * @param {number} fret - Fret to check
     * @returns {boolean} True if within bounds
     */
    contains(fret) {
        return fret >= this.minFret && fret <= this.maxFret;
    }
}

// ============================================================================
// Note Class
// ============================================================================

class Note {
    constructor(stringIdx, fret, numStrings) {
        this.stringIdx = stringIdx;
        this.fret = fret;
        this.numStrings = numStrings;
    }

    /**
     * Get MIDI pitch for this note
     * @returns {number} MIDI pitch
     */
    getMidiPitch() {
        return getPitch(this.stringIdx, this.fret, this.numStrings);
    }

    /**
     * Get pitch class (0-11)
     * @returns {number} Pitch class
     */
    getPitchClass() {
        return this.getMidiPitch() % 12;
    }
}

// ============================================================================
// Fretboard Validator
// ============================================================================

class FretboardValidator {
    /**
     * @param {ScaleManager} scaleManager - Scale manager instance
     * @param {string} instrument - "guitar" or "bass"
     */
    constructor(scaleManager, instrument) {
        this.scaleManager = scaleManager;
        this.instrument = instrument;
        this.numStrings = instrument === "guitar" ? GUITAR_NUM_STRINGS : BASS_NUM_STRINGS;
    }

    /**
     * Check if a note is in the current scale
     * @param {Note} note - Note to check
     * @returns {boolean} True if in scale
     */
    isNoteInScale(note) {
        return this.scaleManager.isMidiPitchValid(note.getMidiPitch());
    }

    /**
     * Get all valid notes on the fretboard for current scale
     * @returns {Note[]} Array of valid notes
     */
    getAllValidNotes() {
        const validNotes = [];

        for (let s = 0; s < this.numStrings; s++) {
            for (let f = MIN_FRET; f <= MAX_FRET; f++) {
                const note = new Note(s, f, this.numStrings);
                if (this.isNoteInScale(note)) {
                    validNotes.push(note);
                }
            }
        }

        return validNotes;
    }
}

// ============================================================================
// Note Generator - Position Box + Pitch Control
// ============================================================================

class NoteGenerator {
    /**
     * @param {FretboardValidator} validator - Fretboard validator
     */
    constructor(validator) {
        this.validator = validator;
        this.positionBox = new PositionBox();
        this.validNotesCache = validator.getAllValidNotes();
        
        // Pitch tracking for global range validation
        this.globalMinPitch = Infinity;
        this.globalMaxPitch = -Infinity;
    }

    /**
     * Generate complete tablature (16 notes)
     * @returns {Note[]} Array of 16 notes
     */
    generateTablature() {
        const notes = [];

        // Reset pitch tracking
        this.globalMinPitch = Infinity;
        this.globalMaxPitch = -Infinity;

        // Generate first note and initialize Position Box
        const firstNote = this.generateFirstNote();
        this.positionBox.initialize(firstNote.fret);
        
        // Update global pitch range with first note
        const firstPitch = firstNote.getMidiPitch();
        this.globalMinPitch = firstPitch;
        this.globalMaxPitch = firstPitch;
        
        notes.push(firstNote);

        // Generate remaining notes within Position Box
        let consecutiveSameString = 0;

        for (let i = 1; i < NUM_NOTES; i++) {
            const previous = notes[i - 1];

            // Force string change after 3 consecutive notes on same string
            const mustChangeString = consecutiveSameString >= MAX_CONSECUTIVE_SAME_STRING;

            const nextNote = this.generateNextNote(previous, consecutiveSameString, mustChangeString, notes);

            // Update global pitch range
            const pitch = nextNote.getMidiPitch();
            if (pitch < this.globalMinPitch) this.globalMinPitch = pitch;
            if (pitch > this.globalMaxPitch) this.globalMaxPitch = pitch;

            // Track consecutive same string
            if (nextNote.stringIdx === previous.stringIdx) {
                consecutiveSameString++;
            } else {
                consecutiveSameString = 0;
            }

            notes.push(nextNote);
        }

        return notes;
    }

    /**
     * Generate the first note of the exercise
     * @returns {Note} First note
     */
    generateFirstNote() {
        const numStrings = this.validator.numStrings;

        // Prefer middle strings and frets for ergonomic starting position
        let stringIdx = Math.floor(Math.random() * (numStrings - 2)) + 1;
        let fret = Math.floor(Math.random() * 8) + 5;  // 5-12

        let attempts = 0;
        let note = new Note(stringIdx, fret, numStrings);

        while (!this.validator.isNoteInScale(note) && attempts < 50) {
            stringIdx = Math.floor(Math.random() * (numStrings - 2)) + 1;
            fret = Math.floor(Math.random() * 8) + 5;
            note = new Note(stringIdx, fret, numStrings);
            attempts++;
        }

        // Fallback: find any valid note
        if (!this.validator.isNoteInScale(note) && this.validNotesCache.length > 0) {
            const randomIdx = Math.floor(Math.random() * this.validNotesCache.length);
            const cached = this.validNotesCache[randomIdx];
            note = new Note(cached.stringIdx, cached.fret, numStrings);
        }

        return note;
    }

    /**
     * Generate the next note based on previous note
     * @param {Note} previous - Previous note
     * @param {number} consecutiveSameString - Count of consecutive notes on same string
     * @param {boolean} mustChangeString - Whether we must change string
     * @param {Note[]} previousNotes - All previous notes (for pitch validation)
     * @returns {Note} Next note
     */
    generateNextNote(previous, consecutiveSameString, mustChangeString, previousNotes) {
        const candidates = this.buildCandidates(previous, mustChangeString, previousNotes);

        // Emergency fallback if no candidates
        if (candidates.length === 0) {
            const fallbackNote = this.findClosestPitchNote(previous, previousNotes);
            if (fallbackNote) {
                return fallbackNote;
            }
            
            // Ultimate fallback: adjacent string, same fret
            const numStrings = this.validator.numStrings;
            const newString = previous.stringIdx < numStrings / 2
                ? previous.stringIdx + 1
                : previous.stringIdx - 1;

            return new Note(
                Math.max(0, Math.min(numStrings - 1, newString)),
                previous.fret,
                numStrings
            );
        }

        // Extract weights
        const weights = candidates.map(c => c.weight);

        // Select candidate based on weights
        const selectedIdx = selectWeighted(weights);
        const selected = candidates[selectedIdx];

        return new Note(selected.note.stringIdx, selected.note.fret, this.validator.numStrings);
    }

    /**
     * Build list of valid candidates with weights
     * @param {Note} previous - Previous note
     * @param {boolean} mustChangeString - Whether we must change string
     * @param {Note[]} previousNotes - All previous notes (for pitch validation)
     * @returns {Array} Array of candidates with weights
     */
    buildCandidates(previous, mustChangeString, previousNotes) {
        const candidates = [];
        const numStrings = this.validator.numStrings;

        // Determine which strings to consider
        const stringsToCheck = [];

        if (mustChangeString) {
            // Must change string - but can skip to ANY string (free string skipping)
            for (let s = 0; s < numStrings; s++) {
                if (s !== previous.stringIdx) {
                    stringsToCheck.push(s);
                }
            }
        } else {
            // Can stay on same string or jump to any other string
            for (let s = 0; s < numStrings; s++) {
                stringsToCheck.push(s);
            }
        }

        // For each string, find valid fret candidates within Position Box
        for (const str of stringsToCheck) {
            for (let fret = this.positionBox.minFret; fret <= this.positionBox.maxFret; fret++) {
                // Skip if same note as previous (must be different)
                if (str === previous.stringIdx && fret === previous.fret) {
                    continue;
                }

                const candidateNote = new Note(str, fret, numStrings);

                // Check if note is in scale
                if (!this.validator.isNoteInScale(candidateNote)) {
                    continue;
                }

                // PITCH CONTROL VALIDATION
                const candidatePitch = candidateNote.getMidiPitch();

                // Rule 1: Local range (last 4 notes + candidate must fit in 1 octave)
                if (!this.isValidForLocalRange(candidateNote, previousNotes)) {
                    continue;
                }

                // Rule 2: Global range (entire exercise must fit in 2 octaves)
                if (!this.isValidForGlobalRange(candidatePitch)) {
                    continue;
                }

                // Calculate fret distance from previous note for weighting
                const fretDistance = Math.abs(fret - previous.fret);

                // Calculate weight based on distance
                let weight = this.calculateWeight(fretDistance);

                // Bonus for staying on same string (promotes fluency when possible)
                if (str === previous.stringIdx) {
                    weight = Math.floor(weight * 1.2);  // 20% bonus
                }

                candidates.push({
                    note: candidateNote,
                    weight: weight,
                    fretDistance: fretDistance
                });
            }
        }

        return candidates;
    }

    /**
     * Check if candidate note fits within local pitch range (1 octave max)
     * @param {Note} candidate - Candidate note
     * @param {Note[]} previousNotes - Previous notes
     * @returns {boolean} True if valid
     */
    isValidForLocalRange(candidate, previousNotes) {
        if (previousNotes.length === 0) return true;

        // Get the last LOCAL_WINDOW_SIZE notes (or fewer if not enough)
        const startIdx = Math.max(0, previousNotes.length - LOCAL_WINDOW_SIZE);
        
        const candidatePitch = candidate.getMidiPitch();
        let minPitch = candidatePitch;
        let maxPitch = candidatePitch;

        // Check against the last N notes
        for (let i = startIdx; i < previousNotes.length; i++) {
            const pitch = previousNotes[i].getMidiPitch();
            if (pitch < minPitch) minPitch = pitch;
            if (pitch > maxPitch) maxPitch = pitch;
        }

        // Local range must not exceed 1 octave (12 semitones)
        return (maxPitch - minPitch) <= MAX_LOCAL_RANGE;
    }

    /**
     * Check if candidate pitch fits within global pitch range (2 octaves max)
     * @param {number} candidatePitch - Candidate pitch (MIDI)
     * @returns {boolean} True if valid
     */
    isValidForGlobalRange(candidatePitch) {
        // Calculate what the new global range would be
        const newMin = Math.min(candidatePitch, this.globalMinPitch);
        const newMax = Math.max(candidatePitch, this.globalMaxPitch);

        // Global range must not exceed 2 octaves (24 semitones)
        return (newMax - newMin) <= MAX_GLOBAL_RANGE;
    }

    /**
     * Calculate weight based on fret distance
     * @param {number} fretDistance - Distance in frets
     * @returns {number} Weight value
     */
    calculateWeight(fretDistance) {
        switch (fretDistance) {
            case 0:
            case 1:
            case 2:
                return WEIGHT_CLOSE;
            case 3:
                return WEIGHT_MEDIUM;
            case 4:
                return WEIGHT_FAR;
            default:
                return 0;
        }
    }

    /**
     * Find the note with closest pitch that passes all validations
     * @param {Note} previous - Previous note
     * @param {Note[]} previousNotes - Previous notes
     * @returns {Note|null} Best note or null
     */
    findClosestPitchNote(previous, previousNotes) {
        const numStrings = this.validator.numStrings;
        const previousPitch = previous.getMidiPitch();
        
        let bestNote = null;
        let bestDistance = Infinity;

        // Search through valid notes cache for closest pitch that passes validations
        for (const cached of this.validNotesCache) {
            // Must be in position box
            if (!this.positionBox.contains(cached.fret)) continue;

            const pitch = cached.getMidiPitch();
            const distance = Math.abs(pitch - previousPitch);

            // Check local range
            if (!this.isValidForLocalRange(cached, previousNotes)) continue;

            // Check global range
            if (!this.isValidForGlobalRange(pitch)) continue;

            if (distance < bestDistance) {
                bestDistance = distance;
                bestNote = cached;
            }
        }

        if (bestNote) {
            return new Note(bestNote.stringIdx, bestNote.fret, numStrings);
        }

        return null;
    }
}

// ============================================================================
// Tablature Formatter
// ============================================================================

/**
 * Format a single note position for a given string
 * @param {Note|null} note - Note or null
 * @param {number} currentString - Current string index
 * @returns {string} Formatted position
 */
function formatNotePosition(note, currentString) {
    if (note === null) {
        return "---";
    }

    if (note.stringIdx === currentString) {
        return note.fret.toString();
    }

    return "---";
}

/**
 * Print complete tablature as ASCII string
 * @param {Note[]} notes - Array of notes
 * @param {number} numStrings - Number of strings
 * @returns {string} ASCII tablature
 */
function formatTablature(notes, numStrings) {
    const labels = numStrings === GUITAR_NUM_STRINGS ? GUITAR_STRING_LABELS : BASS_STRING_LABELS;
    const lines = [];

    for (let stringIdx = 0; stringIdx < numStrings; stringIdx++) {
        let line = labels[stringIdx] + "|";

        for (const note of notes) {
            const position = formatNotePosition(note, stringIdx);

            line += "-";
            if (position.length === 1) {
                line += position + "--";
            } else if (position.length === 2) {
                line += position + "-";
            } else {
                line += position;
            }
        }

        line += "|";
        lines.push(line);
    }

    return lines.join("\n");
}

// ============================================================================
// Main Generator Class - Public API
// ============================================================================

export class TablatureGenerator {
    /**
     * @param {string} instrument - "guitar" or "bass"
     * @param {number|string} rootKey - Key index (0-11) or note name (e.g., "C", "C#")
     * @param {string} scaleName - Scale name (e.g., "Major", "Pentatonic Minor")
     */
    constructor(instrument = "guitar", rootKey = null, scaleName = null) {
        this.instrument = instrument;
        this.scaleManager = new ScaleManager();

        // If specific key/scale provided, use them
        if (rootKey !== null && scaleName !== null) {
            let keyIndex = typeof rootKey === "string"
                ? this.parseKeyName(rootKey)
                : rootKey;

            if (keyIndex >= 0 && keyIndex < 12) {
                this.scaleManager.setKeyAndScale(keyIndex, scaleName);
            }
        }

        this.validator = new FretboardValidator(this.scaleManager, instrument);
        this.noteGenerator = new NoteGenerator(this.validator);
        this.notes = [];
    }

    /**
     * Parse key name to index
     * @param {string} name - Key name
     * @returns {number} Key index or -1
     */
    parseKeyName(name) {
        const NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];
        const upperName = name.toUpperCase();
        return NOTE_NAMES.indexOf(upperName);
    }

    /**
     * Generate new tablature
     * @returns {object} Result object with tab, info, and easterEgg
     */
    generate() {
        this.notes = this.noteGenerator.generateTablature();

        const numStrings = this.instrument === "guitar" ? GUITAR_NUM_STRINGS : BASS_NUM_STRINGS;
        const tab = formatTablature(this.notes, numStrings);
        const keyName = this.scaleManager.getCurrentKeyName();
        const scaleName = this.scaleManager.getCurrentScaleName();
        const scaleNotes = this.scaleManager.getScaleNotes();
        const easterEgg = generateAbsurdFact();

        return {
            tab: tab,
            info: `${keyName} ${scaleName} (${scaleNotes})`,
            easterEgg: easterEgg,
            instrument: this.instrument,
            key: keyName,
            scale: scaleName,
            notes: this.notes.map(n => ({
                string: n.stringIdx,
                fret: n.fret
            }))
        };
    }

    /**
     * Regenerate with same key/scale
     * @returns {object} Result object
     */
    regenerate() {
        // Rebuild validator and generator (Position Box will be re-randomized)
        this.validator = new FretboardValidator(this.scaleManager, this.instrument);
        this.noteGenerator = new NoteGenerator(this.validator);

        return this.generate();
    }

    /**
     * Set specific key and scale
     * @param {number} keyIndex - Key index (0-11)
     * @param {string} scaleName - Scale name
     */
    setKeyAndScale(keyIndex, scaleName) {
        this.scaleManager.setKeyAndScale(keyIndex, scaleName);
        this.validator = new FretboardValidator(this.scaleManager, this.instrument);
        this.noteGenerator = new NoteGenerator(this.validator);
    }

    /**
     * Get current key name
     * @returns {string} Key name
     */
    getCurrentKeyName() {
        return this.scaleManager.getCurrentKeyName();
    }

    /**
     * Get current scale name
     * @returns {string} Scale name
     */
    getCurrentScaleName() {
        return this.scaleManager.getCurrentScaleName();
    }
}

// ============================================================================
// Convenience Function - Generate with one call
// ============================================================================

/**
 * Generate a tablature with specified parameters
 * @param {string} instrument - "guitar" or "bass"
 * @param {string} rootKey - Key name (e.g., "C", "C#", "D")
 * @param {string} scaleName - Scale name
 * @returns {object} Result object with tab, info, and easterEgg
 */
export function generateTablature(instrument = "guitar", rootKey = null, scaleName = null) {
    const generator = new TablatureGenerator(instrument, rootKey, scaleName);
    return generator.generate();
}
