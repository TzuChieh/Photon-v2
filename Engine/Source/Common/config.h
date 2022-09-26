#pragma once

#include <cstddef>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Core Settings                                                             //
///////////////////////////////////////////////////////////////////////////////

/*! @brief Version of Photon.
*/
#define PH_ENGINE_VERSION "2.0.0"

/*! @brief Version of Photon Scene Description Language.
*/
#define PH_PSDL_VERSION "1.0.0"

/*! @brief Enable debug functionalities.
Assertions will be enabled on debug mode.
*/
#define PH_DEBUG

/*! @brief Abort the engine on assertion fail.
*/
#define PH_ABORT_ON_ASSERTION_FAILED

/*! @brief Perform and print a stack trace when assertion failed.
*/
#define PH_PRINT_STACK_TRACE_ON_ASSERTION_FAILED

/*! @brief Assuring floating point types has specified sizes.
*/
#define PH_STRICT_FLOATING_POINT_SIZES

// Log as soon as possible (primarily for debugging).
//#define PH_UNBUFFERED_LOG

// If defined, double precision real number will be used.
//#define PH_USE_DOUBLE_REAL

/*! @brief Enable debug log level.
*/
#define PH_ENABLE_DEBUG_LOG

#define PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS

/*! @brief Default block size for memory arena.
Default value is 512 KiB.
*/
#define PH_MEMORY_ARENA_DEFAULT_BLOCK_SIZE_IN_BYTES (static_cast<std::size_t>(512) * 1024)

#define PH_TFUNCTION_MIN_SIZE_IN_BYTES (static_cast<std::size_t>(32))

/*! @brief Directory that stores engine log file.
*/
#define PH_LOG_FILE_DIRECTRY "./Logs/"

#define PH_CONFIG_DIRECTORY "./Config/"

#define PH_INTERNAL_RESOURCE_DIRECTORY "./InternalResource/"

#define PH_RESOURCE_DIRECTORY "./Photon-v2-Resource-main/Resource/"

///////////////////////////////////////////////////////////////////////////////
// Render Modes                                                              //
///////////////////////////////////////////////////////////////////////////////

#define PH_RENDER_MODE_LINEAR_SRGB
//#define PH_RENDER_MODE_ACES
//#define PH_RENDER_MODE_SPECTRAL
//#define PH_RENDER_MODE_FULL_SPECTRAL

///////////////////////////////////////////////////////////////////////////////
// Data Structures                                                           //
///////////////////////////////////////////////////////////////////////////////

#define PH_SPECTRUM_SAMPLED_MIN_WAVELENGTH_NM 350
#define PH_SPECTRUM_SAMPLED_MAX_WAVELENGTH_NM 850
#define PH_SPECTRUM_SAMPLED_NUM_SAMPLES       100
#define PH_HIT_PROBE_DEPTH                    8

#define PH_SDL_MAX_FIELDS    64
#define PH_SDL_MAX_FUNCTIONS 64

/*! @brief Number of available bytes for a probe's cache.
Note that a byte is not necessarily 8-bit.
*/
#define PH_HIT_PROBE_CACHE_BYTES 12

#define PH_NUMERIC_IMAGE_MAX_ELEMENTS 4

namespace ph
{

class Config final
{
public:
	static std::string& CORE_RESOURCE_DIRECTORY();
};

}// end namespace ph
