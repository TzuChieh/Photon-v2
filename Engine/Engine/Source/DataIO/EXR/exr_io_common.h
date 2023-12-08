#pragma once

/*! @file

@brief IO functions and helpers for processing EXR files.

Also contains helpers for using ILM's OpenEXR library for processing EXR files. The basic idea is that
we provide pixel data in `TFrame`, then create `Imf::Header` and map frame data using `Imf::FrameBuffer`.
Finally, pixel data is read/written to file using `Imf::InputFile`/`Imf::OutputFile`. If the element
type in `TFrame` does not match the precision we requested, then a conversion will be done (converting
to a suitable `TFrame` on the fly, which incurs additional memory allocation).

To simplify common usages, `TFrame` is assumed to contain the full data window. If a display-window only
`TFrame` is desired, use `adjust_frame_to_display_window()`.

Note that `Imf::FrameBuffer` and `Imf::Slice` do not own the specified data, they only reference it.
The original pixel data (typically a `TFrame` here) must be kept alive during the output process.
*/

#include "Frame/frame_fwd.h"

#include "Common/ThirdParty/lib_openexr.h"

#include <array>
#include <string_view>
#include <string>

namespace ph { class Path; }
namespace ph { class ByteBuffer; }

namespace ph
{

#if PH_THIRD_PARTY_HAS_OPENEXR

template<typename T>
struct TImfPixel final
{
	T r;
	T g;
	T b;
	T a;
};

template<typename T>
void copy_frame_to_imf_array(
	const HdrRgbFrame& frame,
	HdrComponent alphaValue,
	Imf::Array2D<TImfPixel<T>>& array2D);

template<typename T, std::size_t N>
void create_imf_header_for_frame(
	Imf::Header& header,
	const TFrame<T, N>& frame,
	const std::array<std::string_view, N>& channelNames);

template<std::size_t N>
auto find_imf_channels(
	const Imf::Header& header,
	const std::array<std::string_view, N>& channelNames)
-> std::array<const Imf::Channel*, N>;

/*!
@param channelNames Name for each channel. Channels without a name will be omitted.
*/
template<typename T, std::size_t N>
void map_imf_framebuffer_to_frame(
	Imf::FrameBuffer& framebuffer,
	const Imf::Header& header,
	const TFrame<T, N>& frame,
	const std::array<std::string_view, N>& channelNames);

//template<typename T, std::size_t N>
//void adjust_frame_to_imf_display_window(
//	TFrame<T, N>& frame, 
//	const Imf::Header& header);

void write_imf_framebuffer_to_filesystem(
	const Imf::Header& header,
	const Imf::FrameBuffer& framebuffer,
	const int numScanlines,
	const Path& filePath);

void write_imf_framebuffer_to_memory(
	const Imf::Header& header,
	const Imf::FrameBuffer& framebuffer,
	const int numScanlines,
	ByteBuffer& buffer);

std::string list_all_imf_layers_and_channels(const Imf::Header& header);

#endif
// end PH_THIRD_PARTY_HAS_OPENEXR

}// end namespace ph

#include "DataIO/EXR/exr_io_common.ipp"
