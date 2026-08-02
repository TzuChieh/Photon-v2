#pragma once

#include "Engine/DataIO/FileSystem/Path.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Utility/INoCopyAndMove.h"
#include "Engine/Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>

namespace ph
{

class ByteBuffer;
class PictureData;

/*! @brief Reads and writes EXR files.
The constructor opens a file and reads its header. `load()` reads selected channels into a new
`PictureData` or caller-owned storage. Static `save()` overloads write `PictureData` to a file or
memory.
*/
class ExrFile final : private INoCopyAndMove
{
public:
	/*! @brief Open @p filePath and read its header.
	*/
	explicit ExrFile(const Path& filePath);
	~ExrFile();

	/*! @brief Return whether the header contains the channel.
	*/
	///@{
	bool hasChannel(const char* channelName) const;
	bool hasChannel(const std::string& channelName) const;
	///@}

	/*! @brief Return the number of channels declared by the header.
	*/
	std::size_t numChannels() const;

	/*! @brief Describe standalone and layered channels for diagnostics.
	*/
	std::string describeChannels() const;

	/*! @brief Return the size of the EXR data window.
	*/
	math::Vector2S getDataWindowSizePx() const;

	/*! @brief Load selected channels as `float32` components.
	Converts supported channel values to `float32` and fills missing named channels with zero.
	Empty names leave the corresponding output component unchanged.
	@param out_components Caller-owned storage. The span must contain exactly
	`getDataWindowSizePx().product() * channelNames.size()` components.
	@param channelNames Standalone or layered channel names in output component order. Non-empty names
	must be unique.
	*/
	void load(
		TSpan<float32> out_components,
		TSpanView<std::string_view> channelNames);

	/*! @brief Load selected channels into a new `PictureData`.
	The picture has one `float32` component per channel name.
	Channel names must be non-empty and unique.
	*/
	PictureData load(TSpanView<std::string_view> channelNames);

	/*! @brief Save @p pictureData to @p filePath.

	The picture must be non-empty with `float32` components. @p channelNames must contain one name
	per component, in component order. An empty name skips that component. A non-empty alpha channel
	name adds a channel filled with @p alphaValue.

	@param saveInHighPrecision Write `float32` channels if true; otherwise, write `float16` channels.
	*/
	///@{
	static void save(
		const PictureData& pictureData,
		const Path& filePath,
		TSpanView<std::string_view> channelNames,
		bool saveInHighPrecision = false,
		std::string_view alphaChannelName = "",
		float32 alphaValue = 1);

	static void save(
		const PictureData& pictureData,
		const Path& filePath,
		std::initializer_list<std::string_view> channelNames,
		bool saveInHighPrecision = false,
		std::string_view alphaChannelName = "",
		float32 alphaValue = 1);
	///@}

	/*! @brief Save @p pictureData as `float32` EXR data in @p out_buffer.

	The picture must be non-empty with `float32` components. @p channelNames must contain one name
	per component, in component order. An empty name skips that component. A non-empty alpha channel
	name adds a channel filled with @p alphaValue.
	*/
	///@{
	static void save(
		const PictureData& pictureData,
		ByteBuffer& out_buffer,
		TSpanView<std::string_view> channelNames,
		std::string_view alphaChannelName = "",
		float32 alphaValue = 1);
		
	static void save(
		const PictureData& pictureData,
		ByteBuffer& out_buffer,
		std::initializer_list<std::string_view> channelNames,
		std::string_view alphaChannelName = "",
		float32 alphaValue = 1);
	///@}

private:
	class InputFileData;

	Path m_filePath;
	std::unique_ptr<InputFileData> m_inputFileData;
};

}// end namespace ph
