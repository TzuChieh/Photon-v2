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

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<EditorSettings>)
	{
		ClassType clazz("editor-settings");
		clazz.docName("Editor Settings");
		clazz.description("Settings for editor.");
		clazz.baseOn<Option>();

		TSdlPath<OwnerType> defaultSceneFile("default-scene-file", &OwnerType::defaultSceneFile);
		defaultSceneFile.description("Scene file that loads automatically if none was specified.");
		clazz.addField(defaultSceneFile);

		return clazz;
	}
};

}// end namespace ph::editor
