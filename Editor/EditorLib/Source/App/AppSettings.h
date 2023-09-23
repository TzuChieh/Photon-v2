#pragma once

#include "RenderCore/SDL/sdl_graphics_api_type.h"

#include <SDL/Option.h>
#include <SDL/sdl_interface.h>
#include <Common/primitive_type.h>
#include <Math/TVector2.h>
#include <Utility/TSpan.h>

#include <string>

namespace ph::editor
{

class AppSettings : public Option
{
public:
	std::string title;
	math::Vector2S displaySizePx;
	uint32 maxFPS;
	ghi::EGraphicsAPI graphicsAPI;
	bool useDebugModeGHI;

	/*! @brief Additional arguments passed from command line.
	Useful if some behavior want to be controlled/overridden by command line arguments.
	*/
	///@{
	TSpanView<const char*> getCmdArgs();
	void setCmdArgs(int argc, char* argv[]);
	///@}

private:
	// Not tracked by SDL
	TSpanView<const char*> m_cmdArgs;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AppSettings>)
	{
		ClassType clazz("app-settings");
		clazz.docName("Application Settings");
		clazz.description("Settings for the application.");
		clazz.baseOn<Option>();

		TSdlString<OwnerType> title("title", &OwnerType::title);
		title.description(
			"Title of the application.");
		title.defaultTo("Photon Editor");
		clazz.addField(title);

		TSdlVector2S<OwnerType> displaySizePx("display-size", &OwnerType::displaySizePx);
		displaySizePx.description(
			"Dimensions of the area for showing content.");
		displaySizePx.defaultTo({0, 0});
		clazz.addField(displaySizePx);

		TSdlUInt32<OwnerType> maxFPS("max-fps", &OwnerType::maxFPS);
		maxFPS.description(
			"Maximum frames per second when displaying content.");
		maxFPS.defaultTo(60);
		clazz.addField(maxFPS);

		TSdlEnumField<OwnerType, ghi::EGraphicsAPI> graphicsAPI("graphics-api", &OwnerType::graphicsAPI);
		graphicsAPI.description(
			"The hardware rendering API to use when displaying content.");
		graphicsAPI.defaultTo(ghi::EGraphicsAPI::OpenGL);
		clazz.addField(graphicsAPI);

		TSdlBool<OwnerType> useDebugModeGHI("use-debug-mode-ghi", &OwnerType::useDebugModeGHI);
		useDebugModeGHI.description(
			"Whether to use GHI with better debug ability. This kind of GHI, if supported, can "
			"introduce extra overhead during runtime.");
		useDebugModeGHI.defaultTo(false);
		clazz.addField(useDebugModeGHI);

		return clazz;
	}
};

}// end namespace ph::editor
