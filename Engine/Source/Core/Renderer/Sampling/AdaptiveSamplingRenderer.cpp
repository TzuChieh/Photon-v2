//#include "Core/Renderer/Sampling/AdaptiveSamplingRenderer.h"
//#include "Core/Renderer/Region/GridScheduler.h"
//#include "Core/Renderer/Sampling/SamplingFilmSet.h"
//#include "Core/Filmic/Vector3Film.h"
//#include "Common/assertion.h"
//#include "Core/Renderer/Region/SpiralScheduler.h"
//#include "Core/Renderer/Region/SpiralGridScheduler.h"
//#include "FileIO/SDL/InputPacket.h"
//#include "Core/Filmic/Film.h"
//#include "World/VisualWorld.h"
//#include "Core/Camera/Camera.h"
//#include "Core/Ray.h"
//#include "Math/constant.h"
//#include "Core/SampleGenerator/SampleGenerator.h"
//#include "FileIO/SDL/SdlResourcePack.h"
//#include "Core/Filmic/HdrRgbFilm.h"
//#include "Core/Renderer/RenderWork.h"
//#include "Core/Renderer/RenderWorker.h"
//#include "Core/Renderer/RendererProxy.h"
//#include "Common/assertion.h"
//#include "Core/Filmic/SampleFilters.h"
//#include "Core/Estimator/BVPTEstimator.h"
//#include "Core/Estimator/BNEEPTEstimator.h"
//#include "Core/Estimator/Integrand.h"
//#include "Core/Filmic/Vector3Film.h"
//#include "Core/Renderer/Region/PlateScheduler.h"
//#include "Core/Renderer/Region/StripeScheduler.h"
//#include "Core/Renderer/Region/GridScheduler.h"
//#include "Utility/FixedSizeThreadPool.h"
//#include "Utility/utility.h"
//
//#include <cmath>
//#include <iostream>
//#include <vector>
//#include <thread>
//#include <chrono>
//#include <functional>
//#include <utility>
//
//namespace ph
//{
//
//void AdaptiveSamplingRenderer::doUpdate(const SdlResourcePack& data)
//{
//	clearWorkData();
//
//	m_scene = &data.visualWorld.getScene();
//	m_camera = data.getCamera().get();
//	m_sampleGenerator = data.getSampleGenerator().get();
//
//	m_estimator->update(*m_scene);
//
//	/*m_films.set<EAttribute::LIGHT_ENERGY>(std::make_unique<HdrRgbFilm>(
//		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));
//	m_films.set<EAttribute::LIGHT_ENERGY_HALF_EFFORT>(std::make_unique<HdrRgbFilm>(
//		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));
//	m_films.set<EAttribute::NORMAL>(std::make_unique<Vector3Film>(
//		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter));*/
//
//	m_renderWorks.resize(numWorkers());
//	for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
//	{
//		m_renderWorks[workerId] = SamplingRenderWork(
//			m_estimator.get(),
//			Integrand(m_scene, m_camera),
//			this);
//	}
//
//	m_totalPaths = 0;
//
//	m_dispatcher = DammertzAdaptiveDispatcher(
//		getRenderWindowPx(),
//		m_precisionStandard,
//		m_numPathsPerRegion);
//
//	m_currentGrid = GridScheduler();
//
//	m_allEffortFrame = HdrRgbFrame(getRenderWidthPx(), getRenderHeightPx());
//	m_halfEffortFrame = HdrRgbFrame(getRenderWidthPx(), getRenderHeightPx());
//
//	m_stoppedWorkers.clear();
//}
//
//void AdaptiveSamplingRenderer::doRender()
//{
//	FixedSizeThreadPool workers(numWorkers());
//
//	while(true)
//	{
//		WorkUnit workUnit;
//		{
//			std::lock_guard<std::mutex> lock(m_rendererMutex);
//
//			if(!m_dispatcher.dispatch(&workUnit))
//			{
//				break;
//			}
//
//			m_currentGrid = GridScheduler(
//				numWorkers(),
//				workUnit,
//				Vector2S(2, 2));
//		}
//		
//		for(uint32 workerId = 0; workerId < numWorkers(); ++workerId)
//		{
//			workers.queueWork(createWork(workers, workerId));
//		}
//
//		workers.waitAllWorks();
//
//		// HACK: assumed region independency; peek is inaccurate
//		/*m_films.get(EAttribute::LIGHT_ENERGY)->develop(m_allEffortFrame, workUnit.getRegion());
//		m_films.get(EAttribute::LIGHT_ENERGY_HALF_EFFORT)->develop(m_halfEffortFrame, workUnit.getRegion());*/
//
//		{
//			std::lock_guard<std::mutex> lock(m_rendererMutex);
//
//			m_dispatcher.analyzeFinishedRegion(workUnit.getRegion(), m_allEffortFrame, m_halfEffortFrame);
//		}
//	}
//}
//
//std::function<void()> AdaptiveSamplingRenderer::createWork(FixedSizeThreadPool& workers, uint32 workerId)
//{
//	return [this, workerId, &workers]()
//	{
//		SamplingRenderWork& renderWork = m_renderWorks[workerId];
//
//		float suppliedFraction = 0.0f;
//		float submittedFraction = 0.0f;
//		while(true)
//		{
//			WorkUnit workUnit;
//			{
//				std::lock_guard<std::mutex> lock(m_rendererMutex);
//
//				if(!m_currentGrid.schedule(&workUnit))
//				{
//					break;
//				}
//				PH_ASSERT_GT(workUnit.getVolume(), 0);
//
//				const std::size_t spp = workUnit.getDepth();
//
//				// HACK
//				//renderWork.setFilms(m_films.genChild(workUnit.getRegion()));
//
//				renderWork.setSampleGenerator(m_sampleGenerator->genCopied(spp));
//				renderWork.setRequestedAttributes(supportedAttributes());
//				renderWork.setDomainPx(workUnit.getRegion());
//			}
//
//			m_suppliedFractionBits.store(
//				bitwise_cast<float, std::uint32_t>(suppliedFraction),
//				std::memory_order_relaxed);
//
//			renderWork.work();
//
//			m_submittedFractionBits.store(
//				bitwise_cast<float, std::uint32_t>(submittedFraction),
//				std::memory_order_relaxed);
//
//			m_totalPaths.fetch_add(renderWork.asyncGetStatistics().numSamplesTaken, std::memory_order_relaxed);
//		}
//	};
//}
//
//void AdaptiveSamplingRenderer::asyncUpdateFilm(HdrRgbFilm* workerFilms, bool isUpdating)
//{
//	std::lock_guard<std::mutex> lock(m_rendererMutex);
//
//	mergeWorkFilms(workerFilms);
//
//	// HACK
//	/*addUpdatedRegion(workerFilms.get<EAttribute::LIGHT_ENERGY>()->getEffectiveWindowPx(), isUpdating);
//	addUpdatedRegion(workerFilms.get<EAttribute::LIGHT_ENERGY_HALF_EFFORT>()->getEffectiveWindowPx(), isUpdating);*/
//}
//
//void AdaptiveSamplingRenderer::clearWorkData()
//{
//	m_updatedRegions.clear();
//
//	// TODO: other data
//}
//
//ERegionStatus AdaptiveSamplingRenderer::asyncPollUpdatedRegion(Region* const out_region)
//{
//	PH_ASSERT(out_region != nullptr);
//
//	std::lock_guard<std::mutex> lock(m_rendererMutex);
//
//	if(m_updatedRegions.empty())
//	{
//		return ERegionStatus::INVALID;
//	}
//
//	const UpdatedRegion updatedRegion = m_updatedRegions.front();
//	m_updatedRegions.pop_front();
//
//	*out_region = updatedRegion.region;
//	if(updatedRegion.isFinished)
//	{
//		return ERegionStatus::FINISHED;
//	}
//	else
//	{
//		return ERegionStatus::UPDATING;
//	}
//}
//
//// FIXME: Peeking does not need to ensure correctness of the frame.
//// If correctness is not guaranteed, develop methods should be reimplemented. 
//// (correctness is guaranteed currently)
//void AdaptiveSamplingRenderer::asyncPeekRegion(
//	HdrRgbFrame&     out_frame,
//	const Region&    region,
//	const EAttribute attribute)
//{
//	std::lock_guard<std::mutex> lock(m_rendererMutex);
//
//	/*const SamplingFilmBase* film = m_films.get(attribute);
//	if(film)
//	{
//		film->develop(out_frame, region);
//	}
//	else
//	{
//		out_frame.fill(0);
//	}*/
//}
//
//void AdaptiveSamplingRenderer::develop(HdrRgbFrame& out_frame, const EAttribute attribute)
//{
//	asyncPeekRegion(out_frame, getRenderWindowPx(), attribute);
//}
//
//void AdaptiveSamplingRenderer::asyncDevelop(HdrRgbFrame& out_frame, EAttribute attribute)
//{
//	asyncPeekRegion(out_frame, getRenderWindowPx(), attribute);
//}
//
//void AdaptiveSamplingRenderer::mergeWorkFilms(HdrRgbFilm* workerFilms)
//{
//	// HACK
//
//	/*const auto& lightFilm = workerFilms.get<EAttribute::LIGHT_ENERGY>();
//	lightFilm->mergeToParent();
//	lightFilm->clear();
//	
//	const auto& normalFilm = workerFilms.get<EAttribute::NORMAL>();
//	normalFilm->mergeToParent();
//	normalFilm->clear();
//
//	const auto& halfEffortFilm = workerFilms.get<EAttribute::LIGHT_ENERGY_HALF_EFFORT>();
//	halfEffortFilm->mergeToParent();
//	halfEffortFilm->clear();*/
//}
//
//void AdaptiveSamplingRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
//{
//	for(UpdatedRegion& pendingRegion : m_updatedRegions)
//	{
//		// later added region takes the precedence
//		if(pendingRegion.region.equals(region))
//		{
//			pendingRegion.isFinished = !isUpdating;
//			return;
//		}
//	}
//
//	m_updatedRegions.push_back(UpdatedRegion{region, !isUpdating});
//}
//
//RenderState AdaptiveSamplingRenderer::asyncQueryRenderState()
//{
//	uint64 totalElapsedMs = 0;
//	uint64 totalNumSamples = 0;
//	for(auto&& work : m_renderWorks)
//	{
//		const auto statistics = work.asyncGetStatistics();
//		totalElapsedMs += work.asyncGetProgress().getElapsedMs();
//		totalNumSamples += statistics.numSamplesTaken;
//	}
//
//	const float32 samplesPerMs = totalElapsedMs != 0 ?
//		static_cast<float32>(m_renderWorks.size() * totalNumSamples) / static_cast<float32>(totalElapsedMs) : 0.0f;
//
//	RenderState state;
//	state.setIntegerState(0, m_totalPaths.load(std::memory_order_relaxed) / static_cast<std::size_t>(getRenderWindowPx().calcArea()));
//	state.setRealState(0, samplesPerMs * 1000);
//	return state;
//}
//
//RenderProgress AdaptiveSamplingRenderer::asyncQueryRenderProgress()
//{
//	RenderProgress workerProgress(0, 0, 0);
//	{
//		for(auto&& work : m_renderWorks)
//		{
//			workerProgress += work.asyncGetProgress();
//		}
//	}
//
//	// HACK
//	const std::size_t totalWork = 100000000;
//	const float suppliedFraction = bitwise_cast<std::uint32_t, float>(m_suppliedFractionBits.load(std::memory_order_relaxed));
//	const float submittedFraction = std::max(bitwise_cast<std::uint32_t, float>(m_submittedFractionBits.load(std::memory_order_relaxed)), suppliedFraction);
//	const float workingFraction = submittedFraction - suppliedFraction;
//	const std::size_t workDone = static_cast<std::size_t>(totalWork * (suppliedFraction + workerProgress.getNormalizedProgress() * workingFraction));
//	RenderProgress totalProgress(totalWork, std::min(workDone, totalWork), workerProgress.getElapsedMs());
//
//	return totalProgress;
//}
//
//AttributeTags AdaptiveSamplingRenderer::supportedAttributes() const
//{
//	PH_ASSERT(m_estimator);
//	return m_estimator->supportedAttributes();
//}
//
//std::string AdaptiveSamplingRenderer::renderStateName(const RenderState::EType type, const std::size_t index) const
//{
//	PH_ASSERT_LT(index, RenderState::numStates(type));
//
//	if(type == RenderState::EType::INTEGER)
//	{
//		switch(index)
//		{
//		case 0:  return "paths/pixel (avg.)";
//		default: return "";
//		}
//	}
//	else if(type == RenderState::EType::REAL)
//	{
//		switch(index)
//		{
//		case 0:  return "paths/second";
//		default: return "";
//		}
//	}
//	else
//	{
//		return "";
//	}
//}
//
//// command interface
//
//AdaptiveSamplingRenderer::AdaptiveSamplingRenderer(const InputPacket& packet) :
//
//	SamplingRenderer(packet),
//
//	//m_films(),
//	m_scene(nullptr),
//	m_sampleGenerator(nullptr),
//	m_estimator(nullptr),
//	m_camera(nullptr),
//	m_updatedRegions(),
//	m_rendererMutex(),
//	m_filter(SampleFilters::createGaussianFilter()),
//	m_requestedAttributes()
//{
//	const std::string filterName = packet.getString("filter-name");
//	m_filter = SampleFilters::create(filterName);
//
//	const std::string estimatorName = packet.getString("estimator", "bneept");
//	if(estimatorName == "bvpt")
//	{
//		m_estimator = std::make_unique<BVPTEstimator>();
//	}
//	else if(estimatorName == "bneept")
//	{
//		m_estimator = std::make_unique<BNEEPTEstimator>();
//	}
//
//	const std::string lightEnergyTag = packet.getString("light-energy-tag", "true");
//	const std::string normalTag = packet.getString("normal-tag", "false");
//	if(lightEnergyTag == "true")
//	{
//		m_requestedAttributes.tag(EAttribute::LIGHT_ENERGY);
//	}
//	if(normalTag == "true")
//	{
//		m_requestedAttributes.tag(EAttribute::NORMAL);
//	}
//
//	/*const std::string regionSchedulerName = packet.getString("region-scheduler", "bulk");
//	if(regionSchedulerName == "bulk")
//	{
//		m_workScheduler = std::make_unique<PlateScheduler>();
//	}*/
//
//	PH_ASSERT(m_estimator);
//
//	// DEBUG
//	//m_precisionStandard = packet.getReal("precision-standard", 1.0_r);
//	m_precisionStandard = packet.getReal("precision-standard", 8.0_r);
//	m_numPathsPerRegion = packet.getInteger("paths-per-region", 16);
//}
//
//SdlTypeInfo AdaptiveSamplingRenderer::ciTypeInfo()
//{
//	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "adaptive-sampling");
//}
//
//void AdaptiveSamplingRenderer::ciRegister(CommandRegister& cmdRegister)
//{
//	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
//	{
//		return std::make_unique<AdaptiveSamplingRenderer>(packet);
//	}));
//}
//
//}// end namespace ph
