#pragma once

#include "RenderCore/ghi_infos.h"

#include "ThirdParty/DearImGui.h"

#include <Math/TVector2.h>
#include <Math/TVector4.h>

#include <memory>
#include <array>
#include <cstddef>
#include <optional>
#include <string_view>

namespace ph { class RegularPicture; }
namespace ph { class Path; }

namespace ph::editor
{

class RenderThreadCaller;
class RendererDetailedTexture;

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

	void imguiImage(
		EImguiImage targetImage,
		const math::Vector2F& sizePx,
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1),
		const math::Vector4F& borderColorRGBA = math::Vector4F(0, 0, 0, 0)) const;

	bool imguiImageButton(
		EImguiImage targetImage, 
		const char* strId,
		const math::Vector2F& sizePx, 
		const math::Vector4F& backgroundColorRGBA = math::Vector4F(0, 0, 0, 0),
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1));

	std::optional<ImTextureID> get(EImguiImage targetImage) const;

	/*void imguiDrawImageButton(
		std::string_view 
		EImguiImage targetImage,
		const math::Vector2F& sizePx,
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1),
		const math::Vector4F& borderColorRGBA = math::Vector4F(0, 0, 0, 0));*/

	void loadImageFile(EImguiImage targetImage, const Path& filePath);
	void addTextures(RenderThreadCaller& caller);
	void removeTextures(RenderThreadCaller& caller);

	// TODO: adapter for image & image drawing (with button?)

private:
	struct ImageEntry final
	{
		mutable GHITextureNativeHandle nativeHandle;
		RendererDetailedTexture* resource;
		std::unique_ptr<RegularPicture> sourcePicture;

		ImageEntry();
		~ImageEntry();
	};

	ImageEntry& getImageEntry(EImguiImage targetImage);
	const ImageEntry& getImageEntry(EImguiImage targetImage) const;

	std::array<ImageEntry, static_cast<std::size_t>(EImguiImage::NUM)> m_imageEntries;
};

}// end namespace ph::editor
