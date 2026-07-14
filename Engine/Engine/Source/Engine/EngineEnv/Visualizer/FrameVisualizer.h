#pragma once

#include "Engine/EngineEnv/Visualizer/Visualizer.h"
#include "Engine/EngineEnv/Visualizer/FilmSetting.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/Geometry/TAABB2D.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/EngineEnv/Visualizer/sdl_visualizer_types.h"
#include "Engine/Core/Filmic/filmic_fwd.h"
#include "Engine/Core/Filmic/SamplingFilmLayer.h"

#include <Common/primitive_type.h>

#include <optional>
#include <vector>
#include <memory>
#include <string>

namespace ph { class SampleFilter; }
namespace ph { class Viewport; }

namespace ph
{

class FrameVisualizer : public Visualizer
{
public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	ESampleFilter getSampleFilter() const;
	bool useTabulatedSampleFilter() const;
	std::optional<math::TAABB2D<int64>> getCropWindowPx() const;
	std::vector<FilmSetting> getFilmSettings() const;

protected:
	SampleFilter makeSampleFilter() const;

	static std::vector<SamplingFilmLayer<math::Spectrum>> makeFilmLayers(
		const std::vector<FilmSetting>& filmSettings,
		const Viewport&                 viewport,
		const SampleFilter&             filter);

private:
	ESampleFilter            m_sampleFilter;
	int64                    m_cropWindowXPx;
	int64                    m_cropWindowYPx;
	int64                    m_cropWindowWPx;
	int64                    m_cropWindowHPx;
	bool                     m_useTabulatedSampleFilter;
	std::vector<FilmSetting> m_filmSettings;

public:
	PH_DEFINE_SDL_CLASS(FrameVisualizer, clazz)
	{
		clazz.typeName("frame");
		clazz.docName("Frame Visualizer");
		clazz.description("A visualizer that produces frames, a typical example is an image.");
		clazz.baseOn<Visualizer>();

		TSdlEnumField<OwnerType, ESampleFilter> sampleFilter("sample-filter", &OwnerType::m_sampleFilter);
		sampleFilter.description(
			"Sample filter for the film sampling process.");
		sampleFilter.defaultTo(ESampleFilter::BlackmanHarris);
		sampleFilter.optional();
		clazz.addField(sampleFilter);

		TSdlBool<OwnerType> useTabulatedSampleFilter(
			"use-tabulated-sample-filter", &OwnerType::m_useTabulatedSampleFilter);
		useTabulatedSampleFilter.description(
			"Use a tabulated approximation for sample filters. Box filter does not support this "
			"as it is already simple enough to have no performance benefit for it.");
		useTabulatedSampleFilter.defaultTo(true);
		useTabulatedSampleFilter.optional();
		clazz.addField(useTabulatedSampleFilter);

		TSdlInt64<OwnerType> cropWindowXPx("rect-x", &OwnerType::m_cropWindowXPx);
		cropWindowXPx.description("X coordinate of the lower-left corner of the film cropping window.");
		cropWindowXPx.defaultTo(0);
		cropWindowXPx.optional();
		clazz.addField(cropWindowXPx);

		TSdlInt64<OwnerType> cropWindowYPx("rect-y", &OwnerType::m_cropWindowYPx);
		cropWindowYPx.description("Y coordinate of the lower-left corner of the film cropping window.");
		cropWindowYPx.defaultTo(0);
		cropWindowYPx.optional();
		clazz.addField(cropWindowYPx);

		TSdlInt64<OwnerType> cropWindowWPx("rect-w", &OwnerType::m_cropWindowWPx);
		cropWindowWPx.description("Width of the film cropping window.");
		cropWindowWPx.defaultTo(0);
		cropWindowWPx.optional();
		clazz.addField(cropWindowWPx);

		TSdlInt64<OwnerType> cropWindowHPx("rect-h", &OwnerType::m_cropWindowHPx);
		cropWindowHPx.description("Height of the film cropping window.");
		cropWindowHPx.defaultTo(0);
		cropWindowHPx.optional();
		clazz.addField(cropWindowHPx);

		TSdlStructArray<FilmSetting, OwnerType> films("films", &OwnerType::m_filmSettings);
		films.description("Ordered list of films for output layers.");
		films.optional();
		clazz.addField(films);
	}
};

// In-header Implementations:

inline ESampleFilter FrameVisualizer::getSampleFilter() const
{
	return m_sampleFilter;
}

inline bool FrameVisualizer::useTabulatedSampleFilter() const
{
	return m_useTabulatedSampleFilter;
}

inline std::optional<math::TAABB2D<int64>> FrameVisualizer::getCropWindowPx() const
{
	if(m_cropWindowXPx == 0 && m_cropWindowYPx == 0 && 
	   m_cropWindowWPx == 0 && m_cropWindowHPx == 0)
	{
		return std::nullopt;
	}
	else
	{
		return math::TAABB2D<int64>(
			{m_cropWindowXPx, m_cropWindowYPx}, 
			{m_cropWindowXPx + m_cropWindowWPx, m_cropWindowYPx + m_cropWindowHPx});
	}
}

}// end namespace ph
