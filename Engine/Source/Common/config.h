#pragma once

///////////////////////////////////////////////////////////////////////////////
// Core Compiling Mode:
//

// Define this for debug mode build. Assertions will be on in this mode.
//
#define PH_DEBUG

// Define this if such behavior is desired.
//
#define PH_ABORT_ON_ASSERTION_FAILED

///////////////////////////////////////////////////////////////////////////////
// Render Mode Selections:
//
#define PH_RENDER_MODE_RGB
//#define PH_RENDER_MODE_SPECTRAL
//#define PH_RENDER_MODE_FULL_SPECTRAL

///////////////////////////////////////////////////////////////////////////////
// Core Engine Structures:
//

// If defined, double precision real number will be used.
//
//#define PH_USE_DOUBLE_REAL

#define PH_SPECTRUM_SAMPLED_MIN_WAVELENGTH_NM 350
#define PH_SPECTRUM_SAMPLED_MAX_WAVELENGTH_NM 850
#define PH_SPECTRUM_SAMPLED_NUM_SAMPLES       100
#define PH_INTERSECTION_PROBE_DEPTH           8
#define PH_INTERSECTION_PROBE_REAL_CACHE_SIZE 8