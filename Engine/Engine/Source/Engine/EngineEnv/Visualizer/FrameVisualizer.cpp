#include "Engine/EngineEnv/Visualizer/FrameVisualizer.h"
#include "Engine/Core/Filmic/SampleFilter.h"
#include "Engine/Core/Filmic/HdrRgbFilm.h"
#include "Engine/Core/Filmic/HdrRgbVarianceFilm.h"
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
		return SampleFilter::makeGaussian();

	case ESampleFilter::MitchellNetravali:
		return SampleFilter::makeMitchellNetravali();

	case ESampleFilter::BlackmanHarris:
		return SampleFilter::makeBlackmanHarris();

	default:
		PH_LOG(FrameVisualizer, Note, "sample filter unspecified, using Blackman-Harris filter");
		return SampleFilter::makeBlackmanHarris();
	}

	return {};
}

std::vector<EFilm> FrameVisualizer::getFilmTypes() const
{
	if(m_filmSettings.empty())
	{
		PH_LOG(FrameVisualizer, Note,
			"film type unspecified, using beauty film");
		return {EFilm::Beauty};
	}

	std::vector<EFilm> filmTypes;
	filmTypes.reserve(m_filmSettings.size());

	std::unordered_set<EFilm> uniqueFilmTypes;
	for(const auto& filmSetting : m_filmSettings)
	{
		const auto filmType = filmSetting.getType();
		if(!uniqueFilmTypes.insert(filmType).second)
		{
			throw SdlLoadError(
				"duplicated film type in film list; per-film dimensions/rect are not supported");
		}

		filmTypes.push_back(filmType);
	}

	return filmTypes;
}

std::vector<SamplingFilmLayer<math::Spectrum>> FrameVisualizer::makeFilmLayers(
	const int64                 actualWidthPx,
	const int64                 actualHeightPx,
	const math::TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter&         filter) const
{
	std::vector<SamplingFilmLayer<math::Spectrum>> filmLayers;
	for(const auto filmType : getFilmTypes())
	{
		SamplingFilmLayer<math::Spectrum> filmLayer;
		filmLayer.name = TSdlEnum<EFilm>{}[filmType];
		if(filmLayer.name.empty())
		{
			throw SdlLoadError("unsupported film type");
		}

		switch(filmType)
		{
		case EFilm::Beauty:
			filmLayer.film = std::make_unique<HdrRgbFilm>(
				actualWidthPx, actualHeightPx, effectiveWindowPx, filter);
			break;

		case EFilm::Variance:
			filmLayer.film = std::make_unique<HdrRgbVarianceFilm>(
				actualWidthPx, actualHeightPx, effectiveWindowPx, filter);
			break;

		default:
			throw SdlLoadError("unsupported film type");
		}

		filmLayers.push_back(std::move(filmLayer));
	}

	return filmLayers;
}

}// end namespace ph
