#pragma once

#include "Engine/EngineEnv/Visualizer/sdl_visualizer_types.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class FilmSetting final
{
public:
	EFilm getType() const;

private:
	EFilm m_type;

public:
	PH_DEFINE_SDL_STRUCT(FilmSetting, ztruct)
	{
		ztruct.typeName("film-setting");
		ztruct.description("Film settings for a single frame output layer.");

		TSdlEnumField<OwnerType, EFilm> type("type", &OwnerType::m_type);
		type.description("Film type for this output layer.");
		type.defaultTo(EFilm::Beauty);
		type.optional();
		ztruct.addField(type);
	}
};

inline EFilm FilmSetting::getType() const
{
	return m_type;
}

}// end namespace ph
