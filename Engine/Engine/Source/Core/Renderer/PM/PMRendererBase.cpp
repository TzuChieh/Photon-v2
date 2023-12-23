#include "Core/Renderer/PM/PMRendererBase.h"
#include "Core/Renderer/RenderRegionStatus.h"
#include "Core/Filmic/SampleFilters.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "World/VisualWorld.h"

#include <Common/profiling.h>

#include <utility>

namespace ph
{

PH_DEFINE_LOG_GROUP(PMRenderer, Renderer);

PMRendererBase::PMRendererBase(
	PMCommonParams commonParams,
	Viewport       viewport,
	SampleFilter   filter,
	uint32         numWorkers)

	: Renderer(viewport, numWorkers)

	, m_filter              (std::move(filter))
	, m_commonParams        (commonParams)

	, m_scene               (nullptr)
	, m_receiver            (nullptr)
	, m_sampleGenerator     (nullptr)

	, m_primaryFilm         ()
	, m_statistics          ()
	, m_isPrimaryFilmUpdated()
{}

void PMRendererBase::doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world)
{
	m_scene = world.getScene();
	m_receiver = cooked.getReceiver();
	m_sampleGenerator = cooked.getSampleGenerator();

	m_primaryFilm = HdrRgbFilm(getRenderWidthPx(), getRenderHeightPx(), getRenderRegionPx(), getFilter());
	m_statistics.zero();
	m_isPrimaryFilmUpdated.clear(std::memory_order_relaxed);
}

std::size_t PMRendererBase::asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions)
{
	PH_PROFILE_SCOPE();

	if(out_regions.empty())
	{
		return 0;
	}

	if(m_isPrimaryFilmUpdated.test(std::memory_order_relaxed))
	{
		out_regions[0] = RenderRegionStatus(getRenderRegionPx(), ERegionStatus::Updating);
		m_isPrimaryFilmUpdated.clear(std::memory_order_relaxed);
		return 1;
	}
	else
	{
		// Report region finished even if the film update flag is off, so we meet the required ordering
		out_regions[0] = RenderRegionStatus(getRenderRegionPx(), ERegionStatus::Finished);
		return 1;
	}
}

void PMRendererBase::asyncDevelopPrimaryFilm(const Region& region, HdrRgbFrame& out_frame)
{
	PH_PROFILE_SCOPE();

	m_primaryFilm->develop(out_frame, region);
}

void PMRendererBase::asyncMergeToPrimaryFilm(const HdrRgbFilm& srcFilm)
{
	PH_PROFILE_SCOPE();

	m_primaryFilm->mergeWith(srcFilm);
	m_isPrimaryFilmUpdated.test_and_set(std::memory_order_relaxed);
}

void PMRendererBase::asyncReplacePrimaryFilm(const HdrRgbFilm& srcFilm)
{
	PH_PROFILE_SCOPE();

	m_primaryFilm.locked(
		[&srcFilm](HdrRgbFilm& primaryFilm)
		{
			primaryFilm.clear();
			primaryFilm.mergeWith(srcFilm);
		});
	m_isPrimaryFilmUpdated.test_and_set(std::memory_order_relaxed);
}

}// end namespace ph
