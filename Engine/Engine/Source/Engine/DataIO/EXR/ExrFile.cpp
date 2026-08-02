#include "Engine/DataIO/EXR/ExrFile.h"

#include "Engine/Common/ThirdParty/lib_openexr.h"
#include "Engine/Frame/PictureData.h"
#include "Engine/Utility/ByteBuffer.h"

#include <Common/assertion.h>
#include <Common/exceptions.h>
#include <Common/io_exceptions.h>
#include <Common/primitive_type.h>
#include <Common/profiling.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <format>
#include <initializer_list>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

namespace ph
{

#if PH_THIRD_PARTY_HAS_OPENEXR

namespace
{

/*! @brief Insert slices for tightly packed, bottom-first pixel data.

Components follow @p channelNames order. Empty names still occupy space in each pixel. @p pixelData
must remain valid while the framebuffer is in use.
*/
template<typename T>
inline void insert_imf_slices(
	Imf::FrameBuffer& out_framebuffer,
	const Imf::Header& header,
	const TSpanView<std::type_identity_t<T>> pixelData,
	const TSpanView<std::string_view> channelNames)
{
	static_assert(std::is_same_v<T, half> || std::is_same_v<T, float>);
	constexpr Imf::PixelType VALUE_TYPE = std::is_same_v<T, half> ? Imf::HALF : Imf::FLOAT;

	// Coordinates are discrete, hence the +1 at the end.
	const Imath::Box2i dataWindow = header.dataWindow();
	const auto dataWidth = dataWindow.max.x - dataWindow.min.x + 1;
	const auto dataHeight = dataWindow.max.y - dataWindow.min.y + 1;
	const auto numComponents = channelNames.size();
	PH_ASSERT(dataWidth > 0 && dataHeight > 0 && numComponents > 0);
	PH_ASSERT_EQ(pixelData.size(), dataWidth * dataHeight * numComponents);

	// OpenEXR calculates each pixel address as `base + x * xStride + y * yStride`.
	// `Slice::Make()` handles non-zero data-window origins without user providing an out-of-window pointer.
	const auto pixelBytes = sizeof(T) * numComponents;
	const auto scanlineBytes = pixelBytes * dataWidth;
	const auto xStride = static_cast<std::ptrdiff_t>(pixelBytes);
	const auto yStride = -static_cast<std::ptrdiff_t>(scanlineBytes);
	const char* byteData = reinterpret_cast<const char*>(pixelData.data());
	const char* topScanlineData = byteData + scanlineBytes * (dataHeight - 1);

#if PH_DEBUG
	// Purposely use all variables to calculate the end of scanline bytes
	const char* scanlineDataEnd = topScanlineData + yStride * (dataHeight - 1) + xStride * dataWidth;
	const T* scanlineDataEndExpected = pixelData.data() + numComponents * dataWidth;
	PH_ASSERT(scanlineDataEnd == reinterpret_cast<const char*>(scanlineDataEndExpected));
#endif

	for(std::size_t channelIdx = 0; channelIdx < numComponents; ++channelIdx)
	{
		// Skip channels without a name
		if(channelNames[channelIdx].empty())
		{
			continue;
		}

		out_framebuffer.insert(
			std::string(channelNames[channelIdx]),
			Imf::Slice::Make(
				VALUE_TYPE,
				topScanlineData + sizeof(T) * channelIdx,
				dataWindow,
				static_cast<std::size_t>(xStride),
				// necessary evil: we need to present `frame` with its last scanline first and
				// use a negative y-stride as OpenEXR expect images in top-down manner; this parameter
				// is unsigned and we relied on proper overflow behavior, see https://lists.aswf.io/g/openexr-dev/topic/openexr_rgbaoutputfile_is/70222932
				static_cast<std::size_t>(yStride)));
	}
}

template<typename T>
inline void insert_imf_slices(
	Imf::FrameBuffer& out_framebuffer,
	const Imf::Header& header,
	const TSpanView<std::type_identity_t<T>> pixelData,
	const std::initializer_list<std::string_view> channelNames)
{
	insert_imf_slices<T>(
		out_framebuffer,
		header,
		pixelData,
		TSpanView<std::string_view>(channelNames.begin(), channelNames.size()));
}

inline void insert_imf_channels(
	Imf::Header& out_header,
	const Imf::PixelType pixelType,
	const TSpanView<std::string_view> channelNames)
{
	// OpenEXR's origin is at the upper left, while Photon stores the bottom scanline first. In
	// OpenEXR coordinates, Photon's first scanline has the largest y-coordinate, so DECREASING_Y
	// stores the scanlines in memory order for efficient I/O. Line order only controls how
	// scanlines are stored; it does not flip the image.
	//
	// INCREASING_Y stores the smallest y-coordinate (top) first. DECREASING_Y stores the largest
	// y-coordinate (bottom) first.
	out_header.lineOrder() = Imf::DECREASING_Y;
	for(const std::string_view channelName : channelNames)
	{
		// Skip channels without a name
		if(!channelName.empty())
		{
			out_header.channels().insert(std::string(channelName), Imf::Channel(pixelType));
		}
	}
}

inline void insert_imf_channels(
	Imf::Header& out_header,
	const Imf::PixelType pixelType,
	const std::initializer_list<std::string_view> channelNames)
{
	insert_imf_channels(
		out_header,
		pixelType,
		TSpanView<std::string_view>(channelNames.begin(), channelNames.size()));
}

inline void write_imf_framebuffer_to_filesystem(
	const Imf::Header& header,
	const Imf::FrameBuffer& framebuffer,
	const int numScanlines,
	const Path& filePath)
{
	Imf::OutputFile output(filePath.toNativeString().c_str(), header);
	output.setFrameBuffer(framebuffer);
	output.writePixels(numScanlines);
}

inline void write_imf_framebuffer_to_memory(
	const Imf::Header& header,
	const Imf::FrameBuffer& framebuffer,
	const int numScanlines,
	ByteBuffer& out_buffer)
{
	Imf::StdOSStream stream;
	Imf::OutputFile output(stream, header);
	output.setFrameBuffer(framebuffer);
	output.writePixels(numScanlines);

	// OPT: implement a custom Imf stream type to write data directly
	std::string imfStreamData = stream.str();
	out_buffer.write(imfStreamData.data(), imfStreamData.size());
}

}// end namespace

#endif

class ExrFile::InputFileData final
{
public:
	explicit InputFileData(const Path& filePath)
		: inputFile(filePath.toNativeString().c_str())
	{}

	Imf::InputFile inputFile;
};

ExrFile::ExrFile(const Path& filePath)
	: m_filePath(filePath)
{
	try
	{
		m_inputFileData = std::make_unique<InputFileData>(m_filePath);
	}
	catch(const std::exception& e)
	{
		throw FileIOError(
			"OpenEXR operation failed: " + std::string(e.what()),
			filePath.toString());
	}
}

ExrFile::~ExrFile() = default;

bool ExrFile::hasChannel(const char* const channelName) const
{
	const Imf::ChannelList& channels = m_inputFileData->inputFile.header().channels();
	return channels.findChannel(channelName) != nullptr;
}

bool ExrFile::hasChannel(const std::string& channelName) const
{
	return hasChannel(channelName.c_str());
}

std::size_t ExrFile::numChannels() const
{
	std::size_t numChannels = 0;
	const Imf::ChannelList& channels = m_inputFileData->inputFile.header().channels();
	for(auto channel = channels.begin(); channel != channels.end(); ++channel)
	{
		++numChannels;
	}
	return numChannels;
}

std::string ExrFile::describeChannels() const
{
	const Imf::ChannelList& channels = m_inputFileData->inputFile.header().channels();

	std::string description;
	for(auto channel = channels.begin(); channel != channels.end(); ++channel)
	{
		const std::string channelName(channel.name());
		if(!channelName.empty() && (
		    channelName.front()   == '.' ||
		    channelName.back()    == '.' ||
		    channelName.find('.') == std::string::npos))
		{
			description += "<standalone channel: " + channelName + ">";
		}
	}

	std::set<std::string> layerNames;
	channels.layers(layerNames);
	for(const auto& layerName : layerNames)
	{
		description += "<layer " + layerName + ": ";

		Imf::ChannelList::ConstIterator channelBegin;
		Imf::ChannelList::ConstIterator channelEnd;
		channels.channelsInLayer(layerName, channelBegin, channelEnd);
		for(auto channel = channelBegin; channel != channelEnd; ++channel)
		{
			description += "[" + std::string(channel.name()) + "]";
		}

		description += ">";
	}

	return description.empty() ? "<none>" : description;
}

math::Vector2S ExrFile::getDataWindowSizePx() const
{
	const Imath::Box2i dataWindow = m_inputFileData->inputFile.header().dataWindow();
	return math::Vector2S(
		dataWindow.max.x - dataWindow.min.x + 1,
		dataWindow.max.y - dataWindow.min.y + 1);
}

void ExrFile::load(
	const TSpan<float32> out_components,
	const TSpanView<std::string_view> channelNames)
{
	PH_PROFILE_SCOPE();

	const Imf::Header& header = m_inputFileData->inputFile.header();
	const auto dataWindowSizePx = getDataWindowSizePx();
	const auto widthPx = dataWindowSizePx.x();
	const auto heightPx = dataWindowSizePx.y();
	const auto expectedNumValues = widthPx * heightPx * channelNames.size();
	if(out_components.size() != expectedNumValues)
	{
		throw FileIOError(std::format(
			"EXR output size mismatch: expected {} values for {} channels, got {}",
			expectedNumValues, channelNames.size(), out_components.size()), m_filePath.toString());
	}

	try
	{
		const Imath::Box2i dataWindow = header.dataWindow();

		// OpenEXR converts channel values to `float32` while reading these slices
		Imf::FrameBuffer framebuffer;
		insert_imf_slices<float32>(
			framebuffer,
			header,
			out_components,
			channelNames);

		m_inputFileData->inputFile.setFrameBuffer(framebuffer);
		m_inputFileData->inputFile.readPixels(dataWindow.min.y, dataWindow.max.y);
	}
	catch(const std::exception& e)
	{
		throw FileIOError(
			"OpenEXR operation failed: " + std::string(e.what()),
			m_filePath.toString());
	}
}

PictureData ExrFile::load(const TSpanView<std::string_view> channelNames)
{
	PictureData pictureData(
		getDataWindowSizePx(),
		channelNames.size(),
		EPicturePixelComponent::Float32);
	load(pictureData.components<float32>(), channelNames);
	return pictureData;
}

void ExrFile::save(
	const PictureData& pictureData,
	const Path& filePath,
	const TSpanView<std::string_view> channelNames,
	const bool saveInHighPrecision,
	const std::string_view alphaChannelName,
	const float32 alphaValue)
{
	PH_PROFILE_SCOPE();

#if PH_THIRD_PARTY_HAS_OPENEXR
	try
	{
		PH_ASSERT(!pictureData.isEmpty());
		PH_ASSERT(pictureData.getComponentType() == EPicturePixelComponent::Float32);
		PH_ASSERT_EQ(pictureData.numComponents(), channelNames.size());

		const Imf::PixelType valueType = saveInHighPrecision ? Imf::FLOAT : Imf::HALF;
		const int dataWidth = static_cast<int>(pictureData.getWidthPx());
		const int dataHeight = static_cast<int>(pictureData.getHeightPx());
		const bool needAlpha = !alphaChannelName.empty();

		Imf::Header header(dataWidth, dataHeight);
		insert_imf_channels(
			header,
			valueType,
			channelNames);
		if(needAlpha)
		{
			insert_imf_channels(header, valueType, {alphaChannelName});
		}

		// `Float32` output uses `pictureData` directly
		if(valueType == Imf::FLOAT)
		{
			const TSpanView<float32> floatComponents = pictureData.getComponents<float32>();

			Imf::FrameBuffer framebuffer;
			insert_imf_slices<float32>(
				framebuffer,
				header,
				floatComponents,
				channelNames);

			std::vector<float32> alphaComponents;
			if(needAlpha)
			{
				// OPT: avoid storing the same alpha value for every pixel
				alphaComponents.resize(floatComponents.size() / pictureData.numComponents(), alphaValue);
				insert_imf_slices<float32>(
					framebuffer,
					header,
					alphaComponents,
					{alphaChannelName});
			}

			write_imf_framebuffer_to_filesystem(
				header,
				framebuffer,
				dataHeight,
				filePath);
		}
		// OpenEXR does not convert types while writing, so `HALF` needs a temporary buffer
		else
		{
			PH_ASSERT(valueType == Imf::HALF);

			const TSpanView<float32> floatComponents = pictureData.getComponents<float32>();
			std::vector<half> halfComponents(floatComponents.size());
			std::transform(floatComponents.begin(), floatComponents.end(), halfComponents.begin(),
				[](const float32 component)
				{
					return half(component);
				});

			Imf::FrameBuffer framebuffer;
			insert_imf_slices<half>(
				framebuffer,
				header,
				halfComponents,
				channelNames);

			std::vector<half> alphaComponents;
			if(needAlpha)
			{
				// OPT: avoid storing the same alpha value for every pixel
				alphaComponents.resize(floatComponents.size() / pictureData.numComponents(), half(alphaValue));
				insert_imf_slices<half>(
					framebuffer,
					header,
					alphaComponents,
					{alphaChannelName});
			}

			write_imf_framebuffer_to_filesystem(
				header,
				framebuffer,
				dataHeight,
				filePath);
		}
	}
	catch(const std::exception& e)
	{
		throw FileIOError(
			"OpenEXR operation failed: " + std::string(e.what()),
			filePath.toString());
	}
#else
	throw IllegalOperationException(
		"OpenEXR library is not available.");
#endif
// end PH_THIRD_PARTY_HAS_OPENEXR
}

void ExrFile::save(
	const PictureData& pictureData,
	const Path& filePath,
	const std::initializer_list<std::string_view> channelNames,
	const bool saveInHighPrecision,
	const std::string_view alphaChannelName,
	const float32 alphaValue)
{
	save(
		pictureData,
		filePath,
		TSpanView<std::string_view>(channelNames.begin(), channelNames.size()),
		saveInHighPrecision,
		alphaChannelName,
		alphaValue);
}

void ExrFile::save(
	const PictureData& pictureData,
	ByteBuffer& out_buffer,
	const TSpanView<std::string_view> channelNames,
	const std::string_view alphaChannelName,
	const float32 alphaValue)
{
	PH_PROFILE_SCOPE();

#if PH_THIRD_PARTY_HAS_OPENEXR
	try
	{
		PH_ASSERT(pictureData.getComponentType() == EPicturePixelComponent::Float32);
		PH_ASSERT_EQ(pictureData.numComponents(), channelNames.size());

		const int dataWidth = static_cast<int>(pictureData.getWidthPx());
		const int dataHeight = static_cast<int>(pictureData.getHeightPx());
		const bool needAlpha = !alphaChannelName.empty();
		const TSpanView<float32> floatComponents = pictureData.getComponents<float32>();

		// Map picture data directly from `pictureData`
		Imf::Header header(dataWidth, dataHeight);
		insert_imf_channels(
			header,
			Imf::FLOAT,
			channelNames);

		Imf::FrameBuffer framebuffer;
		insert_imf_slices<float32>(
			framebuffer,
			header,
			floatComponents,
			channelNames);

		// Optionally allocate a single-channel buffer to store alpha
		std::vector<float32> alphaComponents;
		if(needAlpha)
		{
			// OPT: Avoid storing the same alpha value for every pixel
			alphaComponents.resize(floatComponents.size() / pictureData.numComponents(), alphaValue);
			insert_imf_channels(header, Imf::FLOAT, {alphaChannelName});
			insert_imf_slices<float32>(
				framebuffer,
				header,
				alphaComponents,
				{alphaChannelName});
		}

		write_imf_framebuffer_to_memory(header, framebuffer, dataHeight, out_buffer);
	}
	catch(const std::exception& e)
	{
		throw FileIOError("OpenEXR operation failed: " + std::string(e.what()));
	}
#else
	throw IllegalOperationException(
		"OpenEXR library is not available.");
#endif
// end PH_THIRD_PARTY_HAS_OPENEXR
}

void ExrFile::save(
	const PictureData& pictureData,
	ByteBuffer& out_buffer,
	const std::initializer_list<std::string_view> channelNames,
	const std::string_view alphaChannelName,
	const float32 alphaValue)
{
	save(
		pictureData,
		out_buffer,
		TSpanView<std::string_view>(channelNames.begin(), channelNames.size()),
		alphaChannelName,
		alphaValue);
}

}// end namespace ph
