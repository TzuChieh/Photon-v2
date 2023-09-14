#pragma once

#include "RenderCore/ghi_infos.h"
#include "Render/Query/query_basics.h"
#include "RenderCore/Query/query_basics.h"
#include "Render/Query/GetGraphicsTextureHandle.h"
#include "RenderCore/Query/GetTextureNativeHandle.h"
#include "Render/Content/fwd.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Utility/string_utils.h>
#include <Math/TVector2.h>
#include <Math/TVector4.h>
#include <DataIO/FileSystem/Path.h>

#include <memory>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

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

	ImTextureID get(EImguiImage targetImage) const;

	/*void imguiDrawImageButton(
		std::string_view 
		EImguiImage targetImage,
		const math::Vector2F& sizePx,
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1),
		const math::Vector4F& borderColorRGBA = math::Vector4F(0, 0, 0, 0));*/

	void loadImage(EImguiImage targetImage, const Path& filePath);
	void loadImage(const std::string& imageName, const Path& filePath);
	void createRenderCommands(RenderThreadCaller& caller, render::Scene& scene);
	void cleanupTextures(RenderThreadCaller& caller, render::Scene& scene);

	Editor& getEditor();

	// TODO: adapter for image & image drawing (with button?)

private:
	struct Entry
	{
		render::TextureHandle handle;
		ImTextureID textureID = nullptr;
	};

	struct Loader
	{
		std::string entryName;
		int entryIdx = -1;
		Path fileToLoad;
		render::TQueryOf<render::GetGraphicsTextureHandle> gHandleQuery;
		ghi::TQueryOf<ghi::GetTextureNativeHandle> nHandleQuery;
		bool isFinished = false;
	};

	Entry& getEntry(EImguiImage targetImage);
	const Entry& getEntry(EImguiImage targetImage) const;

	static ImTextureID getTextureIDFromNativeHandle(GHITextureNativeHandle nativeHandle);

	Editor* m_editor;
	std::vector<Loader> m_loaders;
	string_utils::TStdUnorderedStringMap<Entry> m_namedEntries;
	std::array<Entry, enum_size<EImguiImage>()> m_builtinEntries;
};

inline Editor& ImguiImageLibrary::getEditor()
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

inline ImTextureID ImguiImageLibrary::get(EImguiImage targetImage) const
{
	return getEntry(targetImage).textureID;
}

inline auto ImguiImageLibrary::getEntry(EImguiImage targetImage)
-> Entry&
{
	PH_ASSERT_LT(static_cast<std::size_t>(targetImage), m_builtinEntries.size());
	return m_builtinEntries[static_cast<std::size_t>(targetImage)];
}

inline auto ImguiImageLibrary::getEntry(EImguiImage targetImage) const
-> const Entry&
{
	PH_ASSERT_LT(static_cast<std::size_t>(targetImage), m_builtinEntries.size());
	return m_builtinEntries[static_cast<std::size_t>(targetImage)];
}

}// end namespace ph::editor
