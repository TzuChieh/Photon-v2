#pragma once

#include "Engine/EngineEnv/Visualizer/sdl_visualizer_types.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/assertion.h>

#include <string>

namespace ph
{

/*! @brief Configures one renderer output film layer.
The film type defines the layer meaning and output contract.
*/
class FilmSetting final
{
public:
	FilmSetting() = default;
	explicit FilmSetting(EFilm type);

	EFilm getType() const;
	std::string getName() const;
	math::EColorSpace getRendererOutputColorSpace() const;

	/*! @brief Whether the layer contains numeric data rather than color.
	*/
	bool isNumeric() const;

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

inline FilmSetting::FilmSetting(const EFilm type)
	: m_type(type)
{}

inline EFilm FilmSetting::getType() const
{
	return m_type;
}

inline std::string FilmSetting::getName() const
{
	return std::string(TSdlEnum<EFilm>{}[m_type]);
}

inline math::EColorSpace FilmSetting::getRendererOutputColorSpace() const
{
	switch(m_type)
	{
	case EFilm::Beauty:
		if constexpr(math::TColorSpaceDef<math::Spectrum::getColorSpace()>::isTristimulus())
		{
			return math::Spectrum::getColorSpace();
		}
		else
		{
			return math::EColorSpace::Linear_sRGB;
		}

	case EFilm::Variance:
		return math::EColorSpace::Unspecified;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return math::EColorSpace::Unspecified;
	}
}

inline bool FilmSetting::isNumeric() const
{
	switch(m_type)
	{
	case EFilm::Beauty:
		return false;

	case EFilm::Variance:
		return true;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

}// end namespace ph
