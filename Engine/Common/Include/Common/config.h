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
#define PH_PSDL_VERSION "1.1.0"

/*! @brief Enable debug functionalities.
Assertions will be enabled on debug mode.
*/
#ifdef PH_CONFIG_ENABLE_DEBUG
#define PH_DEBUG 1
#else
#define PH_DEBUG 0
#endif

#ifdef PH_CONFIG_ENABLE_PROFILING
#define PH_PROFILING 1
#else
#define PH_PROFILING 0
#endif

/*! @brief Abort the engine on assertion fail.
*/
#define PH_ABORT_ON_ASSERTION_FAILED 1

/*! @brief Perform and print a stack trace when assertion failed.
*/
#define PH_PRINT_STACK_TRACE_ON_ASSERTION_FAILED 1

/*! @brief Assuring floating point types has specified sizes.
*/
#define PH_STRICT_FLOATING_POINT_SIZES 1

// Log as soon as possible (primarily for debugging).
//#define PH_UNBUFFERED_LOG

/*! @brief Use double precision real numbers.
*/
#ifdef PH_CONFIG_DOUBLE_PRECISION_REAL
#define PH_USE_DOUBLE_REAL 1
#else
#define PH_USE_DOUBLE_REAL 0
#endif

/*! @brief Enable debug log level.
*/
#define PH_ENABLE_DEBUG_LOG PH_DEBUG

#define PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS 1

/*! @brief Default block size for memory arena.
Default value is 512 KiB.
*/
#define PH_MEMORY_ARENA_DEFAULT_BLOCK_SIZE_IN_BYTES (static_cast<std::size_t>(512) * 1024)

#define PH_TFUNCTION_DEFAULT_MIN_SIZE_IN_BYTES (static_cast<std::size_t>(64))

/*! @brief Enable statistics recording for hit events.
*/
#define PH_ENABLE_HIT_EVENT_STATS 0

/*! @brief Being strict about object lifetime.
Some compiler versions and different standards may be using an object lifetime model that can be
optimized more aggressively (e.g., constant folding on const instances), which may cause UB or
malfunctions on some low-level code. It is then advisible to turn on this option if such behavior
is observed on the host platform. Note that turning this option off does not mean the source code
will be non-conforming to the standard, it means the opposite--follow the latest standard strictly.
Turning this option on is a fallback when things do not go as planned.
*/
#define PH_STRICT_OBJECT_LIFETIME 0

/*! @brief Directory that stores engine log file.
*/
#define PH_LOG_FILE_DIRECTRY "./Logs/"

#define PH_CONFIG_DIRECTORY "./Config/"
#define PH_SCRIPT_DIRECTORY "./Script/"

/*! @brief Resources that are integrated as part of the renderer.
*/
#define PH_INTERNAL_RESOURCE_DIRECTORY "./InternalResource/"

/*! @brief Resources that are optional for the renderer.
*/
#define PH_RENDERER_RESOURCE_DIRECTORY "./Photon-v2-Resource/Resource/"

///////////////////////////////////////////////////////////////////////////////
// Render Modes                                                              //
///////////////////////////////////////////////////////////////////////////////

#define PH_RENDER_MODE_LINEAR_SRGB 0
#define PH_RENDER_MODE_ACES 1
#define PH_RENDER_MODE_SPECTRAL 2
#define PH_RENDER_MODE_FULL_SPECTRAL 3

#define PH_RENDER_MODE PH_RENDER_MODE_LINEAR_SRGB

/*! @brief Being strict about the symmetricity of importance transport.
There are many sources of asymmetry between light and importance transport, but not all of them can
be handled gracefully. One example is when shading normals are used for light transport, it is
equivalent to using an asymmetric, modified BSDF. However, the correction factor to restore consistency
between light and importance transport can have high variance and makes the result unusable for some
scenes. This option (when disabled) attempt to fix that by introducing a small bias.

See also: "SPPM implementation is not symmetric" https://github.com/mmp/pbrt-v3/issues/209
*/
#define PH_STRICT_ASYMMETRIC_IMPORTANCE_TRANSPORT 0

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
#define PH_HIT_PROBE_CACHE_BYTES 32

#define PH_NUMERIC_IMAGE_MAX_ELEMENTS 4

///////////////////////////////////////////////////////////////////////////////
// Hardware Dependent Instruction Sets                                       //
///////////////////////////////////////////////////////////////////////////////

#ifdef PH_CONFIG_USE_SIMD
#define PH_USE_SIMD 1
#else
#define PH_USE_SIMD 0
#endif

#ifdef PH_CONFIG_HARDWARE_HAS_SSE
#define PH_USE_SSE 1
#else
#define PH_USE_SSE 0
#endif

#ifdef PH_CONFIG_HARDWARE_HAS_SSE2
#define PH_USE_SSE2 1
#else
#define PH_USE_SSE2 0
#endif

#ifdef PH_CONFIG_HARDWARE_HAS_SSE3
#define PH_USE_SSE3 1
#else
#define PH_USE_SSE3 0
#endif

#ifdef PH_CONFIG_HARDWARE_HAS_SSSE3
#define PH_USE_SSSE3 1
#else
#define PH_USE_SSSE3 0
#endif

#ifdef PH_CONFIG_HARDWARE_HAS_SSE4_1
#define PH_USE_SSE4_1 1
#else
#define PH_USE_SSE4_1 0
#endif

#ifdef PH_CONFIG_HARDWARE_HAS_SSE4_2
#define PH_USE_SSE4_2 1
#else
#define PH_USE_SSE4_2 0
#endif

///////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                             //
///////////////////////////////////////////////////////////////////////////////

namespace ph
{

class Config final
{
public:
	static std::string& RENDERER_RESOURCE_DIRECTORY();
};

}// end namespace ph
