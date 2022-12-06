#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/RenderThreadCaller.h"
#include "Render/RTRDetailedTexture2DResource.h"
#include "Render/RenderData.h"

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

std::optional<ImTextureID> ImguiImageLibrary::get(const EImguiImage targetImage) const
{
	const ImageEntry& imageEntry = getImageEntry(targetImage);

	// First check if the handle is already cached locally, load it if not
	if(std::holds_alternative<std::monostate>(imageEntry.nativeHandle) && imageEntry.resource)
	{
		auto optNativeHandle = imageEntry.resource->tryGetNativeHandle();
		if(optNativeHandle.has_value())
		{
			imageEntry.nativeHandle = *optNativeHandle;
		}
	}

	if(std::holds_alternative<uint64>(imageEntry.nativeHandle))
	{
		const uint64 handle = std::get<uint64>(imageEntry.nativeHandle);

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
	ImageEntry& imageEntry = getImageEntry(targetImage);
	imageEntry.sourcePicture = std::make_unique<RegularPicture>(io_utils::load_LDR_picture(filePath));
}

void ImguiImageLibrary::addTextures(RenderThreadCaller& caller)
{
	for(ImageEntry& imageEntry : m_imageEntries)
	{
		if(!imageEntry.sourcePicture)
		{
			continue;
		}

		GHIInfoTextureFormat textureFormat;
		textureFormat.pixelFormat = EGHIInfoPixelFormat::RGBA_8;

		auto textureData = std::make_unique<PictureData>(
			std::move(imageEntry.sourcePicture->getPixels()));

		auto textureResource = std::make_unique<RTRDetailedTexture2DResource>(
			textureFormat, std::move(textureData));

		imageEntry.resource = textureResource.get();
		imageEntry.sourcePicture = nullptr;

		caller.add(
			[textureResource = std::move(textureResource)](RenderData& renderData) mutable
			{
				renderData.scene.addResource(std::move(textureResource));
			});
	}
}

void ImguiImageLibrary::removeTextures(RenderThreadCaller& caller)
{
	for(ImageEntry& imageEntry : m_imageEntries)
	{
		if(!imageEntry.resource)
		{
			continue;
		}

		caller.add(
			[textureResource = imageEntry.resource](RenderData& renderData)
			{
				renderData.scene.removeResource(textureResource);
			});

		imageEntry.resource = nullptr;
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
