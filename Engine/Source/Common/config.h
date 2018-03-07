#pragma once

///////////////////////////////////////////////////////////////////////////////
// Core Compiling Mode:
//
#define PH_DEBUG
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
#define PH_SPECTRUM_SAMPLED_MIN_WAVELENGTH_NM 350
#define PH_SPECTRUM_SAMPLED_MAX_WAVELENGTH_NM 850
#define PH_SPECTRUM_SAMPLED_NUM_SAMPLES       100
#define PH_INTERSECTION_PROBE_DEPTH           8
#define PH_INTERSECTION_PROBE_REAL_CACHE_SIZE 8