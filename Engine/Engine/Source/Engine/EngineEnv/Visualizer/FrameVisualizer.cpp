#include "Engine/EngineEnv/Visualizer/FrameVisualizer.h"
#include "Engine/Core/Filmic/SampleFilter.h"
#include "Engine/Core/Filmic/HdrRgbFilm.h"
#include "Engine/Core/Filmic/HdrRgbVarianceFilm.h"
#include "Engine/Frame/Viewport.h"
#include "Engine/SDL/sdl_exceptions.h"

#include <Common/logging.h>

#include <unordered_set>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(FrameVisualizer, Visualizer);

SampleFilter FrameVisualizer::makeSampleFilter() const
{
	switch(getSampleFilter())
	{
	case ESampleFilter::Box:
		return SampleFilter::makeBox();

	case ESampleFilter::Gaussian:
		return SampleFilter::makeGaussian(useTabulatedSampleFilter());

	case ESampleFilter::MitchellNetravali:
		return SampleFilter::makeMitchellNetravali(useTabulatedSampleFilter());

	case ESampleFilter::BlackmanHarris:
		return SampleFilter::makeBlackmanHarris(useTabulatedSampleFilter());

	default:
		PH_LOG(FrameVisualizer, Note, "sample filter unspecified, using Blackman-Harris filter");
		return SampleFilter::makeBlackmanHarris(useTabulatedSampleFilter());
	}

	return {};
}

std::vector<FilmSetting> FrameVisualizer::getFilmSettings() const
{
	if(m_filmSettings.empty())
	{
		PH_LOG(FrameVisualizer, Note,
			"film type unspecified, using beauty film");
		return {FilmSetting(EFilm::Beauty)};
	}

	std::unordered_set<EFilm> uniqueFilmTypes;
	for(const auto& filmSetting : m_filmSettings)
	{
		const auto filmType = filmSetting.getType();
		if(!uniqueFilmTypes.insert(filmType).second)
		{
			throw SdlLoadError(
				"duplicated film type in film list; per-film dimensions/rect are not supported");
		}
	}

	return m_filmSettings;
}

std::vector<SamplingFilmLayer<math::Spectrum>> FrameVisualizer::makeFilmLayers(
	const std::vector<FilmSetting>& filmSettings,
	const Viewport&                 viewport,
	const SampleFilter&             filter)
{
	const auto frameSizePx       = viewport.getBaseSizePx();
	const auto effectiveWindowPx = viewport.getCroppedRegionPx();

	std::vector<SamplingFilmLayer<math::Spectrum>> filmLayers;
	for(const FilmSetting& filmSetting : filmSettings)
	{
		SamplingFilmLayer<math::Spectrum> filmLayer;
		if(filmSetting.getName().empty())
		{
			throw SdlLoadError("unsupported film type");
		}

		switch(filmSetting.getType())
		{
		case EFilm::Beauty:
			filmLayer.film = std::make_unique<HdrRgbFilm>(
				frameSizePx.x(), frameSizePx.y(), effectiveWindowPx, filter);
			break;

		case EFilm::Variance:
			filmLayer.film = std::make_unique<HdrRgbVarianceFilm>(
				frameSizePx.x(), frameSizePx.y(), effectiveWindowPx, filter);
			break;

		default:
			throw SdlLoadError("unsupported film type");
		}

		filmLayers.push_back(std::move(filmLayer));
	}

	return filmLayers;
}

}// end namespace ph
