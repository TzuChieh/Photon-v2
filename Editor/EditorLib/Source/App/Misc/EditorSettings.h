#pragma once

#include "SDL/Option.h"
#include "SDL/sdl_interface.h"

#include <DataIO/FileSystem/Path.h>

namespace ph::editor
{

class EditorSettings : public Option
{
public:
	Path defaultSceneFile;
	bool loadDefaultSceneOnStartup;
	bool isDevelopmentMode;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<EditorSettings>)
	{
		ClassType clazz("editor-settings");
		clazz.docName("Editor Settings");
		clazz.description("Settings for editor.");
		clazz.baseOn<Option>();

		TSdlPath<OwnerType> defaultSceneFile("default-scene-file", &OwnerType::defaultSceneFile);
		defaultSceneFile.description(
			"Scene file that loads automatically if none was specified.");
		clazz.addField(defaultSceneFile);

		TSdlBool<OwnerType> loadDefaultSceneOnStartup("load-default-scene-on-start-up", &OwnerType::loadDefaultSceneOnStartup);
		loadDefaultSceneOnStartup.description(
			"Load the specified default scene automatically when editor starts.");
		loadDefaultSceneOnStartup.defaultTo(true);
		clazz.addField(loadDefaultSceneOnStartup);

		TSdlBool<OwnerType> isDevelopmentMode("development-mode", &OwnerType::isDevelopmentMode);
		isDevelopmentMode.description(
			"Run the editor in development mode. Additional features such as debugging functionalities "
			"are enabled in this mode.");
		isDevelopmentMode.defaultTo(false);
		clazz.addField(isDevelopmentMode);

		return clazz;
	}
};

}// end namespace ph::editor
