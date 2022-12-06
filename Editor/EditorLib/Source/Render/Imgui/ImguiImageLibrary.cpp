#include "Render/Imgui/ImguiImageLibrary.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/RenderThreadCaller.h"

#include <Frame/RegularPicture.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/io_utils.h>

namespace ph::editor
{

ImguiImageLibrary::~ImguiImageLibrary() = default;

ImguiImageLibrary::ImageEntry::ImageEntry()
	: nativeHandle(std::monostate{})
	, resource(nullptr)
	, sourcePicture(nullptr)
{}

ImguiImageLibrary::ImageEntry::~ImageEntry() = default;

void ImguiImageLibrary::loadImageFile(const EImguiImage targetImage, const Path& filePath)
{
	// TODO
}

void ImguiImageLibrary::addTextures(RenderThreadCaller& caller)
{
	// TODO
}

void ImguiImageLibrary::removeTextures(RenderThreadCaller& caller)
{
	// TODO
}

}// end namespace ph::editor
