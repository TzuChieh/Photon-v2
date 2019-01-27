#include "Core/Renderer/Sampling/AdaptiveSamplingRenderer.h"
#include "Core/Renderer/Region/GridScheduler.h"
#include "Core/Renderer/Sampling/SamplingFilmSet.h"
#include "Core/Filmic/Vector3Film.h"
#include "Common/assertion.h"
#include "Core/Renderer/Region/SpiralScheduler.h"
#include "Core/Renderer/Region/SpiralGridScheduler.h"

namespace ph
{

void AdaptiveSamplingRenderer::doUpdate(const SdlResourcePack& data)
{
	SamplingRenderer::doUpdate(data);

	/*m_workScheduler = std::make_unique<GridScheduler>(
		numWorkers(),
		WorkUnit(Region(getRenderWindowPx()), numAvailableSampleBatches()),
		Vector2S(20, 20));*/

	/*m_workScheduler = std::make_unique<SpiralScheduler>(
		numWorkers(),
		WorkUnit(Region(getRenderWindowPx()), numAvailableSampleBatches()),
		Vector2S(20, 20));*/

	m_workScheduler = std::make_unique<SpiralGridScheduler>(
		numWorkers(),
		WorkUnit(Region(getRenderWindowPx()), numAvailableSampleBatches()),
		50);

	m_workUnits.resize(numWorkers());
}

bool AdaptiveSamplingRenderer::supplyWork(
	const uint32 workerId,
	SamplingRenderWork& work,
	float* const out_suppliedFraction)
{
	PH_ASSERT(m_workScheduler);
	PH_ASSERT_LT(workerId, m_workUnits.size());

	WorkUnit& workUnit = m_workUnits[workerId];
	if(m_workScheduler->schedule(&workUnit))
	{
		*out_suppliedFraction = m_workScheduler->getScheduledFraction();
	}
	else
	{
		*out_suppliedFraction = 1.0f;
		return false;
	}

	// HACK
	/*SamplingFilmSet films;
	films.set<EAttribute::LIGHT_ENERGY>(std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), getFilter()));
	films.set<EAttribute::NORMAL>(std::make_unique<Vector3Film>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), getFilter()));*/

	const std::size_t spp = workUnit.getDepth();

	// HACK
	work.setFilms(getSamplingFilms()->genChild(workUnit.getRegion()));

	work.setSampleGenerator(getSampleGenerator()->genCopied(spp));
	work.setRequestedAttributes(getRequestedAttributes());
	work.setDomainPx(workUnit.getRegion());

	return true;
}

void AdaptiveSamplingRenderer::submitWork(
	const uint32 workerId,
	SamplingRenderWork& work,
	float* const out_submittedFraction)
{
	PH_ASSERT(m_workScheduler);
	PH_ASSERT_LT(workerId, m_workUnits.size());

	m_workScheduler->submit(m_workUnits[workerId]);
	*out_submittedFraction = m_workScheduler->getSubmittedFraction();
}

// command interface

AdaptiveSamplingRenderer::AdaptiveSamplingRenderer(const InputPacket& packet) :

	SamplingRenderer(packet),

	m_workScheduler(nullptr)
{
	//WorkScheduler* scheduler = getWorkScheduler();
	/*scheduler->setNumWorkers(getNumWorkers());
	scheduler->setFullRegion(getRenderWindowPx());
	scheduler->setSppBudget(m_sg->numSampleBatches());
	scheduler->init();*/

	//m_workScheduler = std::make_unique<PlateScheduler>(getNumWorkers(), WorkVolume(Region(getRenderWindowPx()), m_sg->numSampleBatches()));
	//m_workScheduler = std::make_unique<StripeScheduler>(getNumWorkers(), WorkVolume(Region(getRenderWindowPx()), m_sg->numSampleBatches()), math::Y_AXIS);

	/*m_workScheduler = std::make_unique<GridScheduler>(
		getNumWorkers(),
		WorkVolume(Region(getRenderWindowPx()), m_sg->numSampleBatches()),
		Vector2S(20, 20));*/
}

SdlTypeInfo AdaptiveSamplingRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "adaptive-sampling");
}

void AdaptiveSamplingRenderer::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<AdaptiveSamplingRenderer>(packet);
	}));
}

}// end namespace ph
