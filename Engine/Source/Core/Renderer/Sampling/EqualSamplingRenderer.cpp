#include "Core/Renderer/Sampling/EqualSamplingRenderer.h"
#include "Core/Renderer/Region/GridScheduler.h"
#include "Core/Renderer/Sampling/SamplingFilmSet.h"
#include "Core/Filmic/Vector3Film.h"
#include "Common/assertion.h"

namespace ph
{

void EqualSamplingRenderer::doUpdate(const SdlResourcePack& data)
{
	SamplingRenderer::doUpdate(data);

	m_workScheduler = std::make_unique<GridScheduler>(
		numWorkers(),
		WorkUnit(Region(getRenderWindowPx()), numAvailableSampleBatches()),
		Vector2S(20, 20));

	m_workUnits.resize(numWorkers());
}

bool EqualSamplingRenderer::supplyWork(uint32 workerId, SamplingRenderWork& work)
{
	PH_ASSERT(m_workScheduler);
	PH_ASSERT_LT(workerId, m_workUnits.size());

	WorkUnit& workUnit = m_workUnits[workerId];
	if(!m_workScheduler->schedule(&workUnit))
	{
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

void EqualSamplingRenderer::submitWork(uint32 workerId, SamplingRenderWork& work)
{
	PH_ASSERT(m_workScheduler);
	PH_ASSERT_LT(workerId, m_workUnits.size());

	m_workScheduler->submit(m_workUnits[workerId]);
}

// command interface

EqualSamplingRenderer::EqualSamplingRenderer(const InputPacket& packet) : 

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

SdlTypeInfo EqualSamplingRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "equal-sampling");
}

void EqualSamplingRenderer::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<EqualSamplingRenderer>(packet);
	}));
}

}// end namespace ph
