#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/RenderThreadCaller.h"
#include "Render/Texture2D.h"
#include "Render/System.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "RenderCore/GraphicsContext.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Frame/RegularPicture.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/io_utils.h>

#include <algorithm>

namespace ph::editor
{

ImguiImageLibrary::ImguiImageLibrary()
	: m_editor(nullptr)
	, m_loaders()
	, m_retrievers()
	, m_unloadingTextures()
	, m_namedEntries()
	, m_builtinEntries()
{}

ImguiImageLibrary::~ImguiImageLibrary() = default;

void ImguiImageLibrary::initialize(Editor* editor)
{
	PH_ASSERT(editor);
	m_editor = editor;
}

void ImguiImageLibrary::terminate()
{}

void ImguiImageLibrary::imguiImage(
	const EImguiImage targetImage,
	const math::Vector2F& sizePx,
	const math::Vector4F& tintColorRGBA,
	const math::Vector4F& borderColorRGBA) const
{
	imgui::image_with_fallback(
		get(targetImage), sizePx, tintColorRGBA, borderColorRGBA);
}

void ImguiImageLibrary::imguiImage(
	std::string_view imageName,
	const math::Vector2F& sizePx,
	const math::Vector4F& tintColorRGBA,
	const math::Vector4F& borderColorRGBA) const
{
	imgui::image_with_fallback(
		get(imageName), sizePx, tintColorRGBA, borderColorRGBA);
}

bool ImguiImageLibrary::imguiImageButton(
	const char* const strId,
	const EImguiImage targetImage,
	const math::Vector2F& sizePx,
	const math::Vector4F& backgroundColorRGBA,
	const math::Vector4F& tintColorRGBA)
{
	return imgui::image_button_with_fallback(
		strId, get(targetImage), sizePx, backgroundColorRGBA, tintColorRGBA);
}

bool ImguiImageLibrary::imguiImageButton(
	const char* strId,
	std::string_view imageName,
	const math::Vector2F& sizePx,
	const math::Vector4F& backgroundColorRGBA,
	const math::Vector4F& tintColorRGBA)
{
	return imgui::image_button_with_fallback(
		strId, get(imageName), sizePx, backgroundColorRGBA, tintColorRGBA);
}

void ImguiImageLibrary::loadImage(EImguiImage targetImage, const Path& filePath)
{
	PH_ASSERT(!filePath.isEmpty());
	PH_ASSERT(!m_builtinEntries[static_cast<int>(targetImage)].handle);

	m_loaders.push_back({
		.entryIdx = static_cast<int>(targetImage),
		.fileToLoad = filePath,
		.format = ghi::ESizedPixelFormat::RGBA_8});
}

void ImguiImageLibrary::loadImage(
	std::string_view imageName,
	const Path& filePath,
	math::Vector2UI sizePx,
	ghi::ESizedPixelFormat format)
{
	if(imageName.empty() || filePath.isEmpty())
	{
		PH_LOG_WARNING(DearImGui,
			"Cannot load image file with incomplete info: name={}, path={}.", imageName, filePath);
		return;
	}

	if(getEntry(imageName))
	{
		PH_LOG(DearImGui,
			"Overwriting image {}.", imageName);
		unloadImage(imageName);
	}

	PH_ASSERT(!m_namedEntries.contains(imageName));
	m_namedEntries[std::string(imageName)] = Entry{};

	m_loaders.push_back({
		.entryName = std::string(imageName),
		.fileToLoad = filePath,
		.sizePx = sizePx,
		.format = format});
}

void ImguiImageLibrary::loadImage(
	std::string_view imageName,
	math::Vector2UI sizePx,
	ghi::ESizedPixelFormat format)
{
	if(imageName.empty())
	{
		PH_LOG_WARNING(DearImGui,
			"Cannot load image buffer with empty name.", imageName);
		return;
	}

	if(getEntry(imageName))
	{
		PH_LOG(DearImGui,
			"Overwriting image {}.", imageName);
		unloadImage(imageName);
	}

	PH_ASSERT(!m_namedEntries.contains(imageName));
	m_namedEntries[std::string(imageName)] = Entry{};

	m_loaders.push_back({
		.entryName = std::string(imageName),
		.sizePx = sizePx,
		.format = format});
}

void ImguiImageLibrary::unloadImage(std::string_view imageName)
{
	if(imageName.empty())
	{
		return;
	}

	auto entryIter = m_namedEntries.find(imageName);
	if(entryIter != m_namedEntries.end())
	{
		Entry& entry = entryIter->second;
		if(entry.handle)
		{
			m_unloadingTextures.push_back(entry.handle);
		}

		m_namedEntries.erase(entryIter);
	}

	auto loaderIter = std::find_if(m_loaders.begin(), m_loaders.end(),
		[imageName](const Loader& loader)
		{
			return loader.entryName == imageName;
		});
	if(loaderIter != m_loaders.end())
	{
		m_loaders.erase(loaderIter);
	}

	auto retrieverIter = std::find_if(m_retrievers.begin(), m_retrievers.end(), 
		[imageName](const NativeHandleRetriever& r)
		{
			return r.entryName == imageName;
		});
	if(retrieverIter != m_retrievers.end())
	{
		retrieverIter->gHandleQuery->cancel();
		retrieverIter->nHandleQuery->cancel();
		m_retrievers.erase(retrieverIter);
	}
}

void ImguiImageLibrary::createRenderCommands(RenderThreadCaller& caller, render::Scene& scene)
{
	for(Loader& loader : m_loaders)
	{
		render::TextureHandle handle;
		auto finalSizePx = math::Vector2UI{0, 0};
		auto finalFormat = ghi::ESizedPixelFormat::Empty;

		// Load image file
		if(!loader.fileToLoad.isEmpty())
		{
			math::Vector2S sizePx;
			if(!io_utils::load_picture_meta(loader.fileToLoad, &sizePx))
			{
				PH_LOG_WARNING(DearImGui,
					"Cannot load image <{}> for size info. Please make sure the file exists or is a "
					"valid image file.", loader.fileToLoad);
				continue;
			}

			// Use loader's size (user specified) if it is valid
			finalSizePx.x() = loader.sizePx.x() != 0 ? loader.sizePx.x() : static_cast<uint32>(sizePx.x());
			finalSizePx.y() = loader.sizePx.y() != 0 ? loader.sizePx.y() : static_cast<uint32>(sizePx.y());
			
			// Use loader's format (user specified) if it is valid, otherwise use LDR RGBA
			// (commonly we want to view all channels from a LDR file format)
			finalFormat = loader.format != ghi::ESizedPixelFormat::Empty
				? loader.format : ghi::ESizedPixelFormat::RGBA_8;

			ghi::TextureDesc desc;
			desc.setSize2D(finalSizePx);
			desc.format.pixelFormat = finalFormat;

			handle = scene.declareTexture();
			caller.add(
				[fileToLoad = loader.fileToLoad, &scene = scene, handle, desc](render::System& /* sys */)
				{
					scene.createTexture(handle, desc);
					scene.loadPicture(handle, fileToLoad);
				});
		}
		// Load empty image buffer
		else if(!loader.entryName.empty())
		{
			// Use loader's size (user specified) if it is valid
			finalSizePx = loader.sizePx;
			if(loader.sizePx.x() == 0 || loader.sizePx.y() == 0)
			{
				const math::Vector2UI defaultSize(128, 128);

				PH_LOG_WARNING(DearImGui,
					"Image buffer <{}> has invalid size {}. Resetting to {}.",
					loader.entryName, loader.sizePx, defaultSize);

				finalSizePx = defaultSize;
			}

			// Use loader's format (user specified) if it is valid, otherwise use LDR RGB
			// (for buffers we cannot have good default, at least make it colorful)
			finalFormat = loader.format != ghi::ESizedPixelFormat::Empty
				? loader.format : ghi::ESizedPixelFormat::RGB_8;

			ghi::TextureDesc desc;
			desc.setSize2D(finalSizePx);
			desc.format.pixelFormat = loader.format == ghi::ESizedPixelFormat::Empty
				? ghi::ESizedPixelFormat::RGB_8 : loader.format;

			handle = scene.declareTexture();
			caller.add(
				[&scene = scene, handle, desc](render::System& /* sys */)
				{
					scene.createTexture(handle, desc);
				});
		}

		// Register texture handle to the entry
		if(handle)
		{
			Entry& entry = !loader.entryName.empty()
				? m_namedEntries[loader.entryName]
				: m_builtinEntries[loader.entryIdx];

			// Cannot fail with proper resource management (load/unload)
			PH_ASSERT(!entry.handle);

			entry.handle = handle;
			entry.sizePx = finalSizePx;
			entry.format = finalFormat;
		}

		// Start querying native handle
		if(handle)
		{
			auto query = render::Query::autoRetry<render::GetGraphicsTextureHandle>(handle, &scene);
			caller.add(
				[query](render::System& sys)
				{
					sys.addQuery(query);
				});
			m_retrievers.push_back({
				.entryName = std::move(loader.entryName),
				.entryIdx = loader.entryIdx,
				.gHandleQuery = std::move(query)});
		}
	}
	m_loaders.clear();

	for(render::TextureHandle& unloadingTexture : m_unloadingTextures)
	{
		PH_ASSERT(unloadingTexture);

		caller.add(
			[handle = unloadingTexture, &scene = scene](render::System& /* sys */)
			{
				scene.removeTexture(handle);
			});
	}
	m_unloadingTextures.clear();

	for(NativeHandleRetriever& r : m_retrievers)
	{
		if(!r.gHandleQuery.isEmpty() && r.gHandleQuery->isReady())
		{
			PH_ASSERT(r.nHandleQuery.isEmpty());
			r.nHandleQuery = ghi::Query::autoRetry<ghi::GetTextureNativeHandle>(
				r.gHandleQuery->getGraphicsTextureHandle());

			caller.add(
				[query = r.nHandleQuery](render::System& sys)
				{
					sys.getGraphicsContext().addQuery(query);
				});

			r.gHandleQuery.clear();
		}

		if(!r.nHandleQuery.isEmpty() && r.nHandleQuery->isReady())
		{
			ghi::TextureNativeHandle nativeHandle = r.nHandleQuery->getNativeHandle();
			ImTextureID textureID = getTextureIDFromNativeHandle(nativeHandle);
			if(!r.entryName.empty())
			{
				m_namedEntries[r.entryName].textureID = textureID;
			}
			else
			{
				m_builtinEntries[r.entryIdx].textureID = textureID;
			}

			r.nHandleQuery.clear();
			r.isFinished = true;
		}
	}

	// Remove all finished retrievers
	std::erase_if(m_retrievers,
		[](const NativeHandleRetriever& r)
		{
			return r.isFinished;
		});
}

void ImguiImageLibrary::cleanupTextures(RenderThreadCaller& caller, render::Scene& scene)
{
	for(auto& [name, entry] : m_namedEntries)
	{
		if(!entry.handle)
		{
			continue;
		}

		caller.add(
			[handle = entry.handle, &scene = scene](render::System& /* sys */)
			{
				scene.removeTexture(handle);
			});
	}
	m_namedEntries.clear();

	for(Entry& entry : m_builtinEntries)
	{
		if(!entry.handle)
		{
			continue;
		}

		caller.add(
			[handle = entry.handle, &scene = scene](render::System& /* sys */)
			{
				scene.removeTexture(handle);
			});
		entry = Entry{};
	}
}

ImTextureID ImguiImageLibrary::getTextureIDFromNativeHandle(ghi::TextureNativeHandle nativeHandle)
{
	if(std::holds_alternative<uint64>(nativeHandle))
	{
		const uint64 handle = std::get<uint64>(nativeHandle);

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
		return nullptr;
	}
}

}// end namespace ph::editor
