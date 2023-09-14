#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/RenderThreadCaller.h"
#include "Render/Texture2D.h"
#include "Render/DetailedTexture.h"
#include "Render/System.h"
#include "Render/Imgui/ImguiFontLibrary.h"
//#include "Render/Imgui/Font/IconsMaterialDesign.h"
#include "Render/Imgui/Font/IconsMaterialDesignIcons.h"
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
		// Draw a top-filled hourglass to indicate the image is unavailable for now
		ImGui::Text(ICON_MDI_TIMER_SAND);
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
		// Add a top-filled hourglass button to indicate the image is unavailable for now
		return ImGui::Button(ICON_MDI_TIMER_SAND);
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
	PH_ASSERT(!filePath.isEmpty());

	m_loaders.push_back({
		.entryIdx = static_cast<int>(targetImage),
		.fileToLoad = filePath});
}

void ImguiImageLibrary::loadImage(const std::string& imageName, const Path& filePath)
{
	PH_ASSERT(!filePath.isEmpty());

	m_loaders.push_back({
		.entryName = imageName,
		.fileToLoad = filePath});
}

void ImguiImageLibrary::createRenderCommands(RenderThreadCaller& caller, render::Scene& scene)
{
	for(Loader& loader : m_loaders)
	{
		if(!loader.fileToLoad.isEmpty())
		{
			math::Vector2S sizePx;
			if(!io_utils::load_picture_meta(loader.fileToLoad, &sizePx))
			{
				PH_LOG_WARNING(DearImGui,
					"Cannot load image <{}> for size info. Please make sure the file exists.",
					loader.fileToLoad);
				loader.isFinished = true;
				continue;
			}

			GHIInfoTextureDesc desc;
			desc.setSize2D(math::Vector2UI(sizePx));
			desc.format.pixelFormat = EGHISizedPixelFormat::RGB_8;

			render::TextureHandle handle = scene.declareTexture();
			if(!loader.entryName.empty())
			{
				m_namedEntries[loader.entryName].handle = handle;
			}
			else
			{
				m_builtinEntries[loader.entryIdx].handle = handle;
			}

			caller.add(
				[fileToLoad = loader.fileToLoad, handle, desc, &scene](render::System& /* sys */)
				{
					scene.createTexture(handle, render::Texture{.desc = desc});
					scene.loadPicture(handle, fileToLoad);
				});

			loader.fileToLoad.clear();

			PH_ASSERT(loader.gHandleQuery.isEmpty());
			loader.gHandleQuery = render::Query::autoRetry<render::GetGraphicsTextureHandle>(
				handle, &scene);

			caller.add(
				[query = loader.gHandleQuery](render::System& sys)
				{
					sys.addQuery(query);
				});
		}

		if(!loader.gHandleQuery.isEmpty() && loader.gHandleQuery->isReady())
		{
			PH_ASSERT(loader.nHandleQuery.isEmpty());
			loader.nHandleQuery = ghi::Query::autoRetry<ghi::GetTextureNativeHandle>(
				loader.gHandleQuery->getGraphicsTextureHandle());

			caller.add(
				[query = loader.nHandleQuery](render::System& sys)
				{
					sys.getGraphicsContext().addQuery(query);
				});

			loader.gHandleQuery.clear();
		}

		if(!loader.nHandleQuery.isEmpty() && loader.nHandleQuery->isReady())
		{
			GHITextureNativeHandle nativeHandle = loader.nHandleQuery->getNativeHandle();
			ImTextureID textureID = getTextureIDFromNativeHandle(nativeHandle);
			if(!loader.entryName.empty())
			{
				m_namedEntries[loader.entryName].textureID = textureID;
			}
			else
			{
				m_builtinEntries[loader.entryIdx].textureID = textureID;
			}

			loader.nHandleQuery.clear();
			loader.isFinished = true;
		}
	}

	// Remove all finished loaders
	std::erase_if(m_loaders,
		[](const Loader& loader)
		{
			return loader.isFinished;
		});
}

void ImguiImageLibrary::cleanupTextures(RenderThreadCaller& caller, render::Scene& scene)
{
	// TODO: loader

	for(Entry& entry : m_builtinEntries)
	{
		if(!entry.handle)
		{
			continue;
		}

		caller.add(
			[handle = entry.handle, &scene](render::System& /* sys */)
			{
				scene.removeTexture(handle);
			});
	}
}

ImTextureID ImguiImageLibrary::getTextureIDFromNativeHandle(GHITextureNativeHandle nativeHandle)
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
