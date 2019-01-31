#pragma once

#include "Core/Renderer/Renderer.h"

namespace ph
{

class SamplingFilmSet;

class SamplingRenderer : public Renderer, public TCommandInterface<SamplingRenderer>
{
public:
	virtual void asyncUpdateFilm(SamplingFilmSet& workerFilms, bool isUpdating) = 0;

// command interface
public:
	explicit SamplingRenderer(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  renderer          </category>
	<type_name> sampling          </type_name>
	<extend>    renderer.renderer </extend>

	<name> Sampling Renderer </name>
	<description/>

	<command type="creator"/>

	</SDL_interface>
*/