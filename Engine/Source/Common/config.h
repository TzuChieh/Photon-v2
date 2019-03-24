#pragma once

///////////////////////////////////////////////////////////////////////////////
// Core Settings:
//

// Define this for debug mode build. Assertions will be on in this mode.
#define PH_DEBUG

// Define this if such behavior is desired.
#define PH_ABORT_ON_ASSERTION_FAILED

// Perform and print a stack trace when assertion failed.
#define PH_PRINT_STACK_TRACE_ON_ASSERTION_FAILED

// Assuring floating point types has specified sizes.
#define PH_STRICT_FLOATING_POINT_SIZES

// Log as soon as possible (primarily for debugging).
//#define PH_UNBUFFERED_LOG

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
#define PH_HIT_PROBE_DEPTH                    8

// Number of available bytes for a probe's cache. Note that a byte is not 
// necessarily 8-bit.
#define PH_HIT_PROBE_CACHE_BYTES 12

#include <string>

namespace ph
{

class Config final
{
public:
	static std::string& CORE_RESOURCE_DIRECTORY();
};

}// end namespace ph
