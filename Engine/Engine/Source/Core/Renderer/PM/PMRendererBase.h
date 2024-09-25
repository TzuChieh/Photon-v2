#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Filmic/SampleFilter.h"
#include "Core/Renderer/PM/PMAtomicStatistics.h"
#include "Core/Renderer/PM/PMCommonParams.h"
#include "Utility/Concurrent/TSynchronized.h"

#include <Common/logging.h>

#include <memory>
#include <atomic>

namespace ph
{

class Receiver;

PH_DECLARE_LOG_GROUP(PMRenderer);

class PMRendererBase : public Renderer
{
public:
	PMRendererBase(
		PMCommonParams commonParams,
		Viewport       viewport,
		SampleFilter   filter,
		uint32         numWorkers);

	void doRender() override = 0;
	void retrieveFrame(std::size_t layerIndex, HdrRgbFrame& out_frame) override = 0;

	RenderStats asyncQueryRenderStats() override = 0;
	RenderProgress asyncQueryRenderProgress() override = 0;

	void asyncPeekFrame(
		std::size_t layerIndex,
		const Region& region,
		HdrRgbFrame& out_frame) override = 0;

	RenderObservationInfo getObservationInfo() const override = 0;

	/*! @brief Gather cooked data. Perform any required initialization before render.
	*/
	void doUpdate(const CoreCookedUnit& cooked, const VisualWorld& world) override;

	/*! @brief Get updated regions based on the state of primary film.
	*/
	std::size_t asyncPollUpdatedRegions(TSpan<RenderRegionStatus> out_regions) override;

	const SampleFilter& getFilter() const;
	const PMCommonParams& getCommonParams() const;
	const Scene* getScene() const;
	const Receiver* getReceiver() const;
	const SampleGenerator* getSampleGenerator() const;
	TSynchronized<HdrRgbFilm>& getPrimaryFilm();
	PMAtomicStatistics& getStatistics();
	void asyncDevelopPrimaryFilm(const Region& region, HdrRgbFrame& out_frame);
	void asyncMergeToPrimaryFilm(const HdrRgbFilm& srcFilm);
	void asyncReplacePrimaryFilm(const HdrRgbFilm& srcFilm);

private:
	SampleFilter              m_filter;
	PMCommonParams            m_commonParams;

	const Scene*              m_scene;
	const Receiver*           m_receiver;
	const SampleGenerator*    m_sampleGenerator;

	TSynchronized<HdrRgbFilm> m_primaryFilm;
	PMAtomicStatistics        m_statistics;
	std::atomic_flag          m_isPrimaryFilmUpdated;
};

inline const SampleFilter& PMRendererBase::getFilter() const
{
	return m_filter;
}

inline const PMCommonParams& PMRendererBase::getCommonParams() const
{
	return m_commonParams;
}

inline const Scene* PMRendererBase::getScene() const
{
	return m_scene;
}

inline const Receiver* PMRendererBase::getReceiver() const
{
	return m_receiver;
}

inline const SampleGenerator* PMRendererBase::getSampleGenerator() const
{
	return m_sampleGenerator;
}

inline TSynchronized<HdrRgbFilm>& PMRendererBase::getPrimaryFilm()
{
	return m_primaryFilm;
}

inline PMAtomicStatistics& PMRendererBase::getStatistics()
{
	return m_statistics;
}

}// end namespace ph
