#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/RenderThreadCaller.h"
#include "Render/RTRDetailedTexture2DResource.h"
#include "Render/RenderData.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"

#include <Frame/RegularPicture.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/io_utils.h>
#include <Common/assertion.h>

#include <algorithm>

namespace ph::editor
{

ImguiImageLibrary::~ImguiImageLibrary() = default;

ImguiImageLibrary::ImageEntry::ImageEntry()
	: nativeHandle(std::monostate{})
	, resource(nullptr)
	, sourcePicture(nullptr)
{}

ImguiImageLibrary::ImageEntry::~ImageEntry() = default;

void ImguiImageLibrary::imguiImage(
	const EImguiImage targetImage,
	const math::Vector2F& sizePx,
	const math::Vector4F& tintColorRGBA,
	const math::Vector4F& borderColorRGBA) const
{
	const auto optImTextureID = get(targetImage);
	if(!optImTextureID.has_value())
	{
		// Draw a top-filled hourglass to indicate the image is unavailable for now
		ImGui::Text(ICON_MD_HOURGLASS_TOP);
		return;
	}

	ImGui::Image(
		*optImTextureID,
		ImVec2(sizePx.x(), sizePx.y()),
		ImVec2(0, 1),// `uv0` is at upper-left corner
		ImVec2(1, 0),// `uv1` is at lower-right corner
		ImVec4(tintColorRGBA.r(), tintColorRGBA.g(), tintColorRGBA.b(), tintColorRGBA.a()),
		ImVec4(borderColorRGBA.r(), borderColorRGBA.g(), borderColorRGBA.b(), borderColorRGBA.a()));
}

bool ImguiImageLibrary::imguiImageButton(
	const EImguiImage targetImage,
	const char* const strId,
	const math::Vector2F& sizePx,
	const math::Vector4F& backgroundColorRGBA,
	const math::Vector4F& tintColorRGBA)
{
	const auto optImTextureID = get(targetImage);
	if(!optImTextureID.has_value())
	{
		// Add a top-filled hourglass button to indicate the image is unavailable for now
		return ImGui::Button(ICON_MD_HOURGLASS_TOP);
	}

	return ImGui::ImageButton(
		strId,
		*optImTextureID,
		ImVec2(sizePx.x(), sizePx.y()),
		ImVec2(0, 1),// `uv0` is at upper-left corner
		ImVec2(1, 0),// `uv1` is at lower-right corner
		ImVec4(backgroundColorRGBA.r(), backgroundColorRGBA.g(), backgroundColorRGBA.b(), backgroundColorRGBA.a()),
		ImVec4(tintColorRGBA.r(), tintColorRGBA.g(), tintColorRGBA.b(), tintColorRGBA.a()));
}

std::optional<ImTextureID> ImguiImageLibrary::get(const EImguiImage targetImage) const
{
	const ImageEntry& entry = getImageEntry(targetImage);

	// Load the handle if it is not already cached locally
	if(std::holds_alternative<std::monostate>(entry.nativeHandle) && entry.resource)
	{
		auto optNativeHandle = entry.resource->tryGetNativeHandle();
		if(optNativeHandle.has_value())
		{
			entry.nativeHandle = *optNativeHandle;
		}
	}

	if(std::holds_alternative<uint64>(entry.nativeHandle))
	{
		const uint64 handle = std::get<uint64>(entry.nativeHandle);

		static_assert(sizeof(ImTextureID) == sizeof(uint64));

		// Though `ImTextureID` is a pointer type, it expect a non-pointer value to be stored
		// directly in the pointer variable rather than pointed-to
		ImTextureID id;
		std::copy_n(
			reinterpret_cast<const std::byte*>(&handle), sizeof(uint64),
			reinterpret_cast<std::byte*>(&id));
		return id;
	}
	else
	{
		return std::nullopt;
	}
}

void ImguiImageLibrary::loadImageFile(const EImguiImage targetImage, const Path& filePath)
{
	ImageEntry& entry = getImageEntry(targetImage);
	entry.sourcePicture = std::make_unique<RegularPicture>(io_utils::load_LDR_picture(filePath));
}

void ImguiImageLibrary::addTextures(RenderThreadCaller& caller)
{
	for(ImageEntry& entry : m_imageEntries)
	{
		if(!entry.sourcePicture)
		{
			continue;
		}

		GHIInfoTextureFormat textureFormat;
		textureFormat.pixelFormat = EGHIInfoPixelFormat::RGBA_8;

		auto textureData = std::make_unique<PictureData>(
			std::move(entry.sourcePicture->getPixels()));

		auto textureResource = std::make_unique<RTRDetailedTexture2DResource>(
			textureFormat, std::move(textureData));

		entry.resource = textureResource.get();
		entry.sourcePicture = nullptr;

		caller.add(
			[textureResource = std::move(textureResource)](RenderData& renderData) mutable
			{
				renderData.scene.addResource(std::move(textureResource));
			});
	}
}

void ImguiImageLibrary::removeTextures(RenderThreadCaller& caller)
{
	for(ImageEntry& entry : m_imageEntries)
	{
		if(!entry.resource)
		{
			continue;
		}

		caller.add(
			[textureResource = entry.resource](RenderData& renderData)
			{
				renderData.scene.removeResource(textureResource);
			});

		entry.resource = nullptr;
	}
}

auto ImguiImageLibrary::getImageEntry(const EImguiImage targetImage)
-> ImageEntry&
{
	PH_ASSERT_LT(static_cast<std::size_t>(targetImage), m_imageEntries.size());
	return m_imageEntries[static_cast<std::size_t>(targetImage)];
}

auto ImguiImageLibrary::getImageEntry(const EImguiImage targetImage) const
-> const ImageEntry&
{
	PH_ASSERT_LT(static_cast<std::size_t>(targetImage), m_imageEntries.size());
	return m_imageEntries[static_cast<std::size_t>(targetImage)];
}

}// end namespace ph::editor
