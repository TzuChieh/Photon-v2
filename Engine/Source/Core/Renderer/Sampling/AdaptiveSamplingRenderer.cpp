#include "Core/Renderer/Sampling/AdaptiveSamplingRenderer.h"
#include "Core/Renderer/Region/GridScheduler.h"
#include "Core/Renderer/Sampling/SamplingFilmSet.h"
#include "Core/Filmic/Vector3Film.h"
#include "Common/assertion.h"
#include "Core/Renderer/Region/SpiralScheduler.h"
#include "Core/Renderer/Region/SpiralGridScheduler.h"
#include "FileIO/SDL/InputPacket.h"

namespace ph
{

void AdaptiveSamplingRenderer::doUpdate(const SdlResourcePack& data)
{
	SamplingRenderer::doUpdate(data);

	m_pendingRegions = std::queue<Region>();
	m_workingRegions.resize(numWorkers());
	m_workerIdToWorkingRegion.resize(numWorkers());

	m_currentGrid = GridScheduler(
		numWorkers(), 
		WorkUnit(getRenderWindowPx(), m_numPathsPerRegion),
		Vector2S(4, 4));
}

bool AdaptiveSamplingRenderer::supplyWork(
	const uint32 workerId,
	SamplingRenderWork& work,
	float* const out_suppliedFraction)
{
	//WorkUnit workUnit;
	//if(!m_currentGrid.schedule(&workUnit))
	//{
	//	if(m_pendingRegions.empty())
	//	{
	//		return false;
	//	}

	//	const Region newRegion = m_pendingRegions.front();
	//	m_pendingRegions.pop();

	//	m_currentGrid = GridScheduler(
	//		numWorkers(),
	//		WorkUnit(newRegion, m_numPathsPerRegion),
	//		Vector2S(4, 4));

	//	m_currentGrid.schedule(&workUnit);
	//}
	//PH_ASSERT_GT(workUnit.getVolume(), 0);



	//WorkUnit& workUnit = m_workUnits[workerId];
	//if(m_workScheduler->schedule(&workUnit))
	//{
	//	*out_suppliedFraction = m_workScheduler->getScheduledFraction();
	//}
	//else
	//{
	//	*out_suppliedFraction = 1.0f;
	//	return false;
	//}

	//// HACK
	///*SamplingFilmSet films;
	//films.set<EAttribute::LIGHT_ENERGY>(std::make_unique<HdrRgbFilm>(
	//	getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), getFilter()));
	//films.set<EAttribute::NORMAL>(std::make_unique<Vector3Film>(
	//	getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), getFilter()));*/

	//const std::size_t spp = workUnit.getDepth();

	//// HACK
	//work.setFilms(getSamplingFilms()->genChild(workUnit.getRegion()));

	//work.setSampleGenerator(getSampleGenerator()->genCopied(spp));
	//work.setRequestedAttributes(getRequestedAttributes());
	//work.setDomainPx(workUnit.getRegion());

	return true;
}

void AdaptiveSamplingRenderer::submitWork(
	const uint32 workerId,
	SamplingRenderWork& work,
	float* const out_submittedFraction)
{
	/*PH_ASSERT(m_workScheduler);
	PH_ASSERT_LT(workerId, m_workUnits.size());

	m_workScheduler->submit(m_workUnits[workerId]);
	*out_submittedFraction = m_workScheduler->getSubmittedFraction();*/
}

// command interface

AdaptiveSamplingRenderer::AdaptiveSamplingRenderer(const InputPacket& packet) :
	SamplingRenderer(packet)
{
	const real precisionStandard = packet.getReal("precision-standard", 1.0_r);
	m_terminateThreshold = precisionStandard * 0.0002_r;
	m_splitThreshold     = 256.0_r * m_terminateThreshold;

	m_numPathsPerRegion = packet.getInteger("paths-per-region", 1);
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
