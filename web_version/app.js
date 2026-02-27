/**
 * CrazyFingers - Main Application Entry Point
 * ES6 Module - DOM Interaction and Event Handling
 */

import { TablatureGenerator } from './generator.js';

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
 * Initialize DOM element references
 */
function initializeElements() {
    elements.instrument = document.getElementById('instrument');
    elements.rootKey = document.getElementById('rootKey');
    elements.scale = document.getElementById('scale');
    elements.btnGenerate = document.getElementById('btn-generate');
    elements.btnRegenerate = document.getElementById('btn-regenerate');
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
}

// ============================================================================
// Event Handlers
// ============================================================================

/**
 * Handle Generate button click
 */
function handleGenerate() {
    const instrument = elements.instrument.value;
    const rootKey = elements.rootKey.value;
    const scale = elements.scale.value;

    console.log('Generating with:', { instrument, rootKey, scale });

    try {
        // Create new generator based on settings
        if (rootKey === 'random' || scale === 'random') {
            // Random generation
            currentGenerator = new TablatureGenerator(instrument);
        } else {
            // Specific key and scale
            currentGenerator = new TablatureGenerator(instrument, rootKey, scale);
        }

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
