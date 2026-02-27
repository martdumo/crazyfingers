/**
 * CrazyFingers - Main Application Entry Point
 * ES6 Module - DOM Interaction and Event Handling
 */

import { TablatureGenerator } from './generator.js';
import { getAllScaleNames, NOTE_NAMES, SCALE_DICTIONARY } from './musicTheory.js';

// ============================================================================
// State
// ============================================================================

let currentGenerator = null;

// ============================================================================
// DOM Elements
// ============================================================================

const elements = {
    instrument: null,
    rootKey: null,
    scale: null,
    btnGenerate: null,
    btnRegenerate: null,
    btnCopy: null,
    resultContainer: null,
    output: null,
    infoKey: null,
    infoScale: null,
    infoNotes: null,
    easterEgg: null,
    errorContainer: null,
    errorMessage: null
};

// ============================================================================
// Initialization
// ============================================================================

/**
 * Resolve a value that might be 'random' to an actual value
 * @param {string} value - Selected value or 'random'
 * @param {string} type - 'key' or 'scale'
 * @returns {string|number} Resolved value
 */
function resolveRandomValue(value, type) {
    if (value !== 'random') {
        return value;  // User selected a specific value
    }

    if (type === 'key') {
        // Random key: select random index from NOTE_NAMES (0-11)
        const randomIndex = Math.floor(Math.random() * NOTE_NAMES.length);
        return randomIndex;  // Return as number (generator accepts both)
    }

    if (type === 'scale') {
        // Random scale: select random key from SCALE_DICTIONARY
        const scaleNames = Object.keys(SCALE_DICTIONARY);
        const randomIndex = Math.floor(Math.random() * scaleNames.length);
        return scaleNames[randomIndex];  // Return as string
    }

    return value;
}

/**
 * Populate the scale dropdown dynamically with all available scales
 */
function populateScaleDropdown() {
    const scaleSelect = elements.scale;
    if (!scaleSelect) return;

    // Get all scale names and sort alphabetically
    const scaleNames = getAllScaleNames();
    scaleNames.sort((a, b) => a.localeCompare(b));

    // Clear existing options (keep the first "random" option)
    scaleSelect.innerHTML = '<option value="random">🎲 Aleatorio</option>';

    // Create and append options for each scale
    for (const name of scaleNames) {
        const option = document.createElement('option');
        option.value = name;
        option.textContent = name;
        scaleSelect.appendChild(option);
    }

    console.log(`✅ Loaded ${scaleNames.length} scales`);
}

/**
 * Initialize DOM element references
 */
function initializeElements() {
    elements.instrument = document.getElementById('instrument');
    elements.rootKey = document.getElementById('rootKey');
    elements.scale = document.getElementById('scale');
    elements.btnGenerate = document.getElementById('btn-generate');
    elements.btnRegenerate = document.getElementById('btn-regenerate');
    elements.btnCopy = document.getElementById('btn-copy');
    elements.resultContainer = document.getElementById('result-container');
    elements.output = document.getElementById('output');
    elements.infoKey = document.getElementById('info-key');
    elements.infoScale = document.getElementById('info-scale');
    elements.infoNotes = document.getElementById('info-notes');
    elements.easterEgg = document.getElementById('easter-egg');
    elements.errorContainer = document.getElementById('error-container');
    elements.errorMessage = document.getElementById('error-message');
}

/**
 * Attach event listeners
 */
function attachEventListeners() {
    elements.btnGenerate.addEventListener('click', handleGenerate);
    elements.btnRegenerate.addEventListener('click', handleRegenerate);
    elements.btnCopy.addEventListener('click', handleCopyToClipboard);
}

// ============================================================================
// Event Handlers
// ============================================================================

/**
 * Handle Generate button click
 */
function handleGenerate() {
    const instrument = elements.instrument.value;
    const selectedKey = elements.rootKey.value;
    const selectedScale = elements.scale.value;

    // Resolve 'random' values to actual values
    const resolvedKey = resolveRandomValue(selectedKey, 'key');
    const resolvedScale = resolveRandomValue(selectedScale, 'scale');

    console.log('Selected:', { key: selectedKey, scale: selectedScale });
    console.log('Resolved:', { key: resolvedKey, scale: resolvedScale });

    try {
        // Create generator with resolved values (always specific key/scale now)
        // If both were random, resolvedKey will be a number (0-11) and resolvedScale will be a string
        currentGenerator = new TablatureGenerator(instrument, resolvedKey, resolvedScale);

        const result = currentGenerator.generate();
        displayResult(result);
        hideError();
    } catch (error) {
        console.error('Error generating tablature:', error);
        showError(error);
    }
}

/**
 * Handle Regenerate button click (same key/scale, new variation)
 */
function handleRegenerate() {
    if (!currentGenerator) {
        // If no generator exists, trigger a new generation
        handleGenerate();
        return;
    }

    try {
        const result = currentGenerator.regenerate();
        displayResult(result);
        hideError();
    } catch (error) {
        console.error('Error regenerating tablature:', error);
        showError(error);
    }
}

// ============================================================================
// Display Functions
// ============================================================================

/**
 * Handle copy to clipboard button click
 */
function handleCopyToClipboard() {
    const textToCopy = elements.output.textContent;

    if (!textToCopy) {
        console.error('No hay contenido para copiar');
        return;
    }

    navigator.clipboard.writeText(textToCopy)
        .then(() => {
            // Feedback visual de éxito
            const originalText = elements.btnCopy.innerHTML;
            elements.btnCopy.innerHTML = '✅ ¡Copiado!';
            elements.btnCopy.style.background = 'var(--accent-celeste)';
            elements.btnCopy.style.color = 'var(--bg-base)';

            setTimeout(() => {
                elements.btnCopy.innerHTML = originalText;
                elements.btnCopy.style.background = '';
                elements.btnCopy.style.color = '';
            }, 2000);
        })
        .catch(err => {
            console.error('Error al copiar: ', err);
            // Fallback para navegadores antiguos
            fallbackCopyToClipboard(textToCopy);
        });
}

/**
 * Fallback copy method for older browsers
 * @param {string} text - Text to copy
 */
function fallbackCopyToClipboard(text) {
    const textArea = document.createElement('textarea');
    textArea.value = text;
    textArea.style.position = 'fixed';
    textArea.style.left = '-999999px';
    document.body.appendChild(textArea);
    textArea.select();
    try {
        document.execCommand('copy');
        const originalText = elements.btnCopy.innerHTML;
        elements.btnCopy.innerHTML = '✅ ¡Copiado!';
        setTimeout(() => {
            elements.btnCopy.innerHTML = originalText;
        }, 2000);
    } catch (err) {
        console.error('Fallback copy failed: ', err);
    }
    document.body.removeChild(textArea);
}

/**
 * Display the generated tablature result
 * @param {object} result - Result object from generator
 */
function displayResult(result) {
    // Show result container
    elements.resultContainer.classList.remove('hidden');
    elements.errorContainer.classList.add('hidden');

    // Format output with instrument header
    const instrumentLabel = result.instrument === 'guitar'
        ? '[Electric Guitar - 6 strings, Standard Tuning (E2-A2-D3-G3-B3-E4)]'
        : '[Bass Guitar - 4 strings, Standard Tuning (E1-A1-D2-G2)]';

    elements.output.textContent = `${instrumentLabel}\n\n${result.tab}`;

    // Show copy button
    elements.btnCopy.style.display = 'inline-flex';

    // Update harmonic info
    elements.infoKey.textContent = result.key;
    elements.infoScale.textContent = result.scale;

    // Extract just the notes from the info string
    const notesMatch = result.info.match(/\(([^)]+)\)/);
    elements.infoNotes.textContent = notesMatch ? notesMatch[1] : result.info;

    // Update easter egg
    elements.easterEgg.textContent = `🎸 ${result.easterEgg}`;

    // Scroll to result
    elements.resultContainer.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
}

/**
 * Show error message
 * @param {Error} error - Error object
 */
function showError(error) {
    elements.errorContainer.classList.remove('hidden');
    elements.resultContainer.classList.add('hidden');
    
    const errorText = `${error.name}: ${error.message}\n\nStack trace:\n${error.stack}`;
    elements.errorMessage.textContent = errorText;
    
    // Also show error in output for visibility
    elements.output.textContent = `⚠️ Error al generar la tablatura:\n\n${error.message}`;
    elements.resultContainer.classList.remove('hidden');
}

/**
 * Hide error container
 */
function hideError() {
    elements.errorContainer.classList.add('hidden');
}

// ============================================================================
// Application Entry Point
// ============================================================================

/**
 * Initialize the application
 */
function initializeApp() {
    console.log('🎸 CrazyFingers - Argentinian Night Edition');
    console.log('Initializing...');

    try {
        initializeElements();
        populateScaleDropdown();  // Load all scales dynamically
        attachEventListeners();
        console.log('✅ Application initialized successfully');
    } catch (error) {
        console.error('❌ Failed to initialize application:', error);
        // Try to show error even if initialization fails
        const output = document.getElementById('output');
        const errorContainer = document.getElementById('error-container');
        if (output) {
            output.textContent = `Error de inicialización: ${error.message}`;
        }
        if (errorContainer) {
            errorContainer.classList.remove('hidden');
            const errorMessage = document.getElementById('error-message');
            if (errorMessage) {
                errorMessage.textContent = error.stack;
            }
        }
    }
}

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', initializeApp);
