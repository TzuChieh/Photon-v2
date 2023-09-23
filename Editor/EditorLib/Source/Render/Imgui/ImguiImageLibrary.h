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

	ImTextureID get(std::string_view name) const;
	ImTextureID get(EImguiImage targetImage) const;

	/*void imguiDrawImageButton(
		std::string_view 
		EImguiImage targetImage,
		const math::Vector2F& sizePx,
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1),
		const math::Vector4F& borderColorRGBA = math::Vector4F(0, 0, 0, 0));*/

	void loadImage(EImguiImage targetImage, const Path& filePath);
	void loadImage(const std::string& imageName, const Path& filePath);

	void loadImage(
		const std::string& imageName, 
		const Path& filePath,
		math::Vector2UI sizePx,
		EGHISizedPixelFormat format);

	void loadImage(
		const std::string& imageName,
		math::Vector2UI sizePx, 
		EGHISizedPixelFormat format);

	void createRenderCommands(RenderThreadCaller& caller, render::Scene& scene);
	void cleanupTextures(RenderThreadCaller& caller, render::Scene& scene);

	Editor& getEditor();

	// TODO: wrapper for image & image drawing (with button?)

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
		math::Vector2UI sizePx = {0, 0};
		EGHISizedPixelFormat format = EGHISizedPixelFormat::Empty;
		bool isProcessed = false;
	};

	struct NativeHandleRetriever
	{
		std::string entryName;
		int entryIdx = -1;
		render::TQueryOf<render::GetGraphicsTextureHandle> gHandleQuery;
		ghi::TQueryOf<ghi::GetTextureNativeHandle> nHandleQuery;
		bool isFinished = false;
	};

	static ImTextureID getTextureIDFromNativeHandle(GHITextureNativeHandle nativeHandle);

	Editor* m_editor;
	std::vector<Loader> m_loaders;
	std::vector<NativeHandleRetriever> m_retrievers;
	string_utils::TStdUnorderedStringMap<Entry> m_namedEntries;
	std::array<Entry, enum_size<EImguiImage>()> m_builtinEntries;
};

inline Editor& ImguiImageLibrary::getEditor()
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

inline ImTextureID ImguiImageLibrary::get(std::string_view name) const
{
	auto iter = m_namedEntries.find(name);
	return iter != m_namedEntries.end() ? iter->second.textureID : nullptr;
}

inline ImTextureID ImguiImageLibrary::get(EImguiImage targetImage) const
{
	PH_ASSERT_LT(static_cast<std::size_t>(targetImage), m_builtinEntries.size());
	return m_builtinEntries[static_cast<std::size_t>(targetImage)].textureID;
}

}// end namespace ph::editor
