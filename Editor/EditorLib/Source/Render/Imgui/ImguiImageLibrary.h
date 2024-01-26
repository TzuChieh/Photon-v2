#pragma once

#include "RenderCore/ghi_infos.h"
#include "Render/Query/query_basics.h"
#include "RenderCore/Query/query_basics.h"
#include "Render/Query/GetGraphicsTextureHandle.h"
#include "RenderCore/Query/GetTextureNativeHandle.h"
#include "Render/Content/fwd.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <Container/TStdUnorderedStringMap.h>
#include <Utility/utility.h>
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

	void imguiImage(
		std::string_view imageName,
		const math::Vector2F& sizePx,
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1),
		const math::Vector4F& borderColorRGBA = math::Vector4F(0, 0, 0, 0)) const;

	bool imguiImageButton(
		const char* strId,
		EImguiImage targetImage, 
		const math::Vector2F& sizePx, 
		const math::Vector4F& backgroundColorRGBA = math::Vector4F(0, 0, 0, 0),
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1));

	bool imguiImageButton(
		const char* strId,
		std::string_view imageName,
		const math::Vector2F& sizePx,
		const math::Vector4F& backgroundColorRGBA = math::Vector4F(0, 0, 0, 0),
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1));

	ImTextureID get(std::string_view imageName) const;
	ImTextureID get(EImguiImage targetImage) const;

	/*! @brief Whether the library has the image.
	If `true` is returned, does not mean `get(imageName)` will necessary return a valid ID. The image
	may still being loaded and an invalid ID (0) may be returned.
	*/
	bool has(std::string_view imageName) const;

	/*! @brief Get information of a named image.
	*/
	///@{
	math::Vector2UI getSizePx(std::string_view imageName) const;
	ghi::ESizedPixelFormat getFormat(std::string_view imageName) const;
	render::TextureHandle getHandle(std::string_view imageName) const;
	ghi::TextureHandle getGraphicsHandle(std::string_view imageName) const;
	///@}

	/*void imguiDrawImageButton(
		std::string_view 
		EImguiImage targetImage,
		const math::Vector2F& sizePx,
		const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1),
		const math::Vector4F& borderColorRGBA = math::Vector4F(0, 0, 0, 0));*/

	void loadImage(EImguiImage targetImage, const Path& filePath);

	void loadImage(
		std::string_view imageName,
		const Path& filePath,
		math::Vector2UI sizePx = {0, 0},
		ghi::ESizedPixelFormat format = ghi::ESizedPixelFormat::Empty);

	void loadImage(
		std::string_view imageName,
		math::Vector2UI sizePx, 
		ghi::ESizedPixelFormat format);

	void unloadImage(std::string_view imageName);

	void createRenderCommands(RenderThreadCaller& caller, render::Scene& scene);

	/*! @brief Unload all named and builtin images.
	*/
	void cleanupTextures(RenderThreadCaller& caller, render::Scene& scene);

	Editor& getEditor();

	// TODO: wrapper for image & image drawing (with button?)

private:
	struct Entry
	{
		render::TextureHandle handle;
		ghi::TextureHandle gHandle;
		ImTextureID textureID = nullptr;
		math::Vector2UI sizePx = {0, 0};
		ghi::ESizedPixelFormat format = ghi::ESizedPixelFormat::Empty;
	};

	struct Loader
	{
		std::string entryName;
		int entryIdx = -1;
		Path fileToLoad;
		math::Vector2UI sizePx = {0, 0};
		ghi::ESizedPixelFormat format = ghi::ESizedPixelFormat::Empty;
	};

	struct NativeHandleRetriever
	{
		std::string entryName;
		int entryIdx = -1;
		render::TQueryOf<render::GetGraphicsTextureHandle> gHandleQuery;
		ghi::TQueryOf<ghi::GetTextureNativeHandle> nHandleQuery;
		bool isFinished = false;
	};

	auto getEntry(std::string_view name) const -> const Entry*;

	static ImTextureID getTextureIDFromNativeHandle(ghi::TextureNativeHandle nativeHandle);

	Editor* m_editor;
	std::vector<Loader> m_loaders;
	std::vector<NativeHandleRetriever> m_retrievers;
	std::vector<render::TextureHandle> m_unloadingTextures;
	TStdUnorderedStringMap<Entry> m_namedEntries;
	std::array<Entry, enum_size<EImguiImage>()> m_builtinEntries;
};

inline math::Vector2UI ImguiImageLibrary::getSizePx(std::string_view name) const
{
	const Entry* entry = getEntry(name);
	return entry ? entry->sizePx : math::Vector2UI{0, 0};
}

inline ghi::ESizedPixelFormat ImguiImageLibrary::getFormat(std::string_view name) const
{
	const Entry* entry = getEntry(name);
	return entry ? entry->format : ghi::ESizedPixelFormat::Empty;
}

inline render::TextureHandle ImguiImageLibrary::getHandle(std::string_view name) const
{
	const Entry* entry = getEntry(name);
	return entry ? entry->handle : render::TextureHandle{};
}

inline ghi::TextureHandle ImguiImageLibrary::getGraphicsHandle(std::string_view name) const
{
	const Entry* entry = getEntry(name);
	return entry ? entry->gHandle : ghi::TextureHandle{};
}

inline Editor& ImguiImageLibrary::getEditor()
{
	PH_ASSERT(m_editor);
	return *m_editor;
}

inline ImTextureID ImguiImageLibrary::get(std::string_view imageName) const
{
	auto iter = m_namedEntries.find(imageName);
	return iter != m_namedEntries.end() ? iter->second.textureID : nullptr;
}

inline ImTextureID ImguiImageLibrary::get(EImguiImage targetImage) const
{
	PH_ASSERT_LT(static_cast<std::size_t>(targetImage), m_builtinEntries.size());
	return m_builtinEntries[static_cast<std::size_t>(targetImage)].textureID;
}

inline bool ImguiImageLibrary::has(std::string_view imageName) const
{
	return getEntry(imageName) != nullptr;
}

inline auto ImguiImageLibrary::getEntry(std::string_view imageName) const
-> const Entry*
{
	auto iter = m_namedEntries.find(imageName);
	return iter != m_namedEntries.end() ? &(iter->second) : nullptr;
}

}// end namespace ph::editor
