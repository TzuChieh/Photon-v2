#pragma once

#include "RenderCore/ghi_infos.h"
#include "RenderCore/Query/query_basics.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Math/TVector2.h>
#include <Math/TVector4.h>

#include <memory>
#include <array>
#include <cstddef>
#include <optional>
#include <string_view>

namespace ph { class RegularPicture; }
namespace ph { class Path; }
namespace ph::editor::render { class DetailedTexture; }
namespace ph::editor::render { class Scene; }

namespace ph::editor
{

class Editor;
class RenderThreadCaller;

/*! @brief Built-in images.
*/
enum class EImguiImage
{
	Warning = 0,
	Folder,
	File,
	Image,

	/*! Number of enum entries. Must follow the last usable entry. */
	SIZE
};

/*!
Dear ImGui **does not** handle image loading. Images are loaded by Photon and handles are stored
here for imgui. Utilities that can adapt to missing images are provided.
*/
class ImguiImageLibrary final
{
public:
	ImguiImageLibrary();
	~ImguiImageLibrary();

	void initialize(Editor* editor);
	void terminate();

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
	void createTextures(RenderThreadCaller& caller, render::Scene& scene);
	void removeTextures(RenderThreadCaller& caller, render::Scene& scene);

	Editor& getEditor();

	// TODO: adapter for image & image drawing (with button?)

private:
	struct ImageEntry final
	{
		mutable GHITextureNativeHandle nativeHandle;
		GHITextureHandle textureHandle;
		ghi::Query nativeHandleQuery;
		std::unique_ptr<RegularPicture> sourcePicture;

		// TODO: remove
		render::DetailedTexture* resource = nullptr;

		ImageEntry();
		~ImageEntry();
	};

	ImageEntry& getImageEntry(EImguiImage targetImage);
	const ImageEntry& getImageEntry(EImguiImage targetImage) const;

	Editor* m_editor;
	std::array<ImageEntry, enum_size<EImguiImage>()> m_builtinEntries;
};

inline Editor& ImguiImageLibrary::getEditor()
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

}// end namespace ph::editor
