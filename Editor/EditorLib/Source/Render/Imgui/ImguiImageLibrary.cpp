#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/RenderThreadCaller.h"
#include "Render/Texture2D.h"
#include "Render/System.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/Font/imgui_icons.h"
#include "RenderCore/GraphicsContext.h"

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
	auto textureID = get(targetImage);
	if(!textureID)
	{
		// Indicate the image is unavailable for now
		ImGui::TextUnformatted(PH_IMGUI_LOADING_ICON " Loading...");
		return;
	}

	ImGui::Image(
		textureID,
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
	auto textureID = get(targetImage);
	if(!textureID)
	{
		// Indicate the image is unavailable for now
		ImGui::TextUnformatted(PH_IMGUI_LOADING_ICON " Loading...");
	}

	return ImGui::ImageButton(
		strId,
		textureID,
		ImVec2(sizePx.x(), sizePx.y()),
		ImVec2(0, 1),// `uv0` is at upper-left corner
		ImVec2(1, 0),// `uv1` is at lower-right corner
		ImVec4(backgroundColorRGBA.r(), backgroundColorRGBA.g(), backgroundColorRGBA.b(), backgroundColorRGBA.a()),
		ImVec4(tintColorRGBA.r(), tintColorRGBA.g(), tintColorRGBA.b(), tintColorRGBA.a()));
}

void ImguiImageLibrary::loadImage(EImguiImage targetImage, const Path& filePath)
{
	if(filePath.isEmpty())
	{
		return;
	}

	m_loaders.push_back({
		.entryIdx = static_cast<int>(targetImage),
		.fileToLoad = filePath,
		.format = ghi::ESizedPixelFormat::RGBA_8});
}

void ImguiImageLibrary::loadImage(const std::string& imageName, const Path& filePath)
{
	if(filePath.isEmpty())
	{
		return;
	}

	m_loaders.push_back({
		.entryName = imageName,
		.fileToLoad = filePath});
}

void ImguiImageLibrary::loadImage(
	const std::string& imageName,
	const Path& filePath,
	math::Vector2UI sizePx,
	ghi::ESizedPixelFormat format)
{
	if(filePath.isEmpty())
	{
		return;
	}

	m_loaders.push_back({
		.entryName = imageName,
		.fileToLoad = filePath,
		.sizePx = sizePx,
		.format = format});
}

void ImguiImageLibrary::loadImage(
	const std::string& imageName,
	math::Vector2UI sizePx,
	ghi::ESizedPixelFormat format)
{
	m_loaders.push_back({
		.entryName = imageName,
		.sizePx = sizePx,
		.format = format});
}

void ImguiImageLibrary::createRenderCommands(RenderThreadCaller& caller, render::Scene& scene)
{
	for(Loader& loader : m_loaders)
	{
		PH_ASSERT(!loader.isProcessed);

		render::TextureHandle handle;

		// Load image file
		if(!loader.fileToLoad.isEmpty())
		{
			math::Vector2S sizePx;
			if(!io_utils::load_picture_meta(loader.fileToLoad, &sizePx))
			{
				PH_LOG_WARNING(DearImGui,
					"Cannot load image <{}> for size info. Please make sure the file exists.",
					loader.fileToLoad);
				loader.isProcessed = true;
				continue;
			}

			ghi::TextureDesc desc;
			sizePx.x() = loader.sizePx.x() != 0 ? loader.sizePx.x() : sizePx.x();
			sizePx.y() = loader.sizePx.y() != 0 ? loader.sizePx.y() : sizePx.y();
			desc.setSize2D(math::Vector2UI(sizePx));
			desc.format.pixelFormat = loader.format == ghi::ESizedPixelFormat::Empty
				? ghi::ESizedPixelFormat::RGBA_8 : loader.format;

			handle = scene.declareTexture();
			caller.add(
				[fileToLoad = loader.fileToLoad, &scene = scene, handle, desc](render::System& /* sys */)
				{
					scene.createTexture(handle, render::Texture{.desc = desc});
					scene.loadPicture(handle, fileToLoad);
				});
		}
		// Load empty image buffer
		else if(!loader.entryName.empty())
		{
			if(loader.sizePx.x() == 0 || loader.sizePx.y() == 0)
			{
				const math::Vector2UI defaultSize(128, 128);

				PH_LOG_WARNING(DearImGui,
					"Image buffer <{}> has invalid size {}. Resetting to {}.",
					loader.entryName, loader.sizePx, defaultSize);

				loader.sizePx = defaultSize;
			}

			ghi::TextureDesc desc;
			desc.setSize2D(loader.sizePx);
			desc.format.pixelFormat = loader.format == ghi::ESizedPixelFormat::Empty
				? ghi::ESizedPixelFormat::RGB_8 : loader.format;

			handle = scene.declareTexture();
			caller.add(
				[&scene = scene, handle, desc](render::System& /* sys */)
				{
					scene.createTexture(handle, render::Texture{.desc = desc});
				});
		}

		// Register texture handle to the entry
		if(handle)
		{
			if(!loader.entryName.empty())
			{
				m_namedEntries[loader.entryName].handle = handle;
			}
			else
			{
				m_builtinEntries[loader.entryIdx].handle = handle;
			}
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

		loader.isProcessed = true;
	}

	// Remove all processed loaders
	std::erase_if(m_loaders,
		[](const Loader& loader)
		{
			return loader.isProcessed;
		});

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
