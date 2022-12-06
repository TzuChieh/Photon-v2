#pragma once

#include "RenderCore/GHITexture2D.h"

#include <memory>
#include <array>
#include <cstddef>

namespace ph { class RegularPicture; }
namespace ph { class Path; }

namespace ph::editor
{

class RenderThreadCaller;
class RTRDetailedTexture2DResource;

enum class EImguiImage
{
	Warning = 0,
	Folder,
	File,
	Image,

	/*! Number of enum entries. Must follow the last usable entry. */
	NUM
};

/*!
Dear ImGui **does not** handle image loading. Images are loaded by Photon and handles are stored
here for imgui. Utilities that can adapt to missing images are provided.
*/
class ImguiImageLibrary final
{
public:
	~ImguiImageLibrary();

	void loadImageFile(EImguiImage targetImage, const Path& filePath);

	void addTextures(RenderThreadCaller& caller);
	void removeTextures(RenderThreadCaller& caller);

	// TODO: adapter for image & image drawing (with button?)

private:
	struct ImageEntry final
	{
		GHITexture2D::NativeHandle nativeHandle;
		RTRDetailedTexture2DResource* resource;
		std::unique_ptr<RegularPicture> sourcePicture;

		ImageEntry();
		~ImageEntry();
	};

	std::array<ImageEntry, static_cast<std::size_t>(EImguiImage::NUM)> m_imageEntries;
};

}// end namespace ph::editor
