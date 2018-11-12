#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "FileIO/SDL/SdlResourcePack.h"
#include "Core/Renderer/PM/TViewpointGatheringWork.h"
#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/VPMRadianceEvaluationWork.h"
#include "Core/Renderer/PM/VPMPhoton.h"
#include "FileIO/SDL/InputPacket.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Utility/concurrent.h"
#include "Common/Logger.h"
#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Core/Renderer/PM/PPMPhoton.h"
#include "Core/Renderer/PM/PPMViewpoint.h"

#include <numeric>

namespace ph
{

namespace
{
	const Logger logger(LogSender("PM Renderer"));
}

void PMRenderer::doUpdate(const SdlResourcePack& data)
{
	m_film = std::make_unique<HdrRgbFilm>(getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

	m_scene = &(data.visualWorld.getScene());
	m_camera = data.getCamera().get();
	m_sg = data.getSampleGenerator().get();

	m_statistics.zero();
	
	/*Scene* scene,
		Camera* camera,
		std::unique_ptr<SampleGenerator> sampleGenerator,
		Viewpoint* viewpointBuffer,
		std::size_t numViewpoints*/

	/*std::size_t numPixels = getRenderWindowPx().calcArea() * m_perPixelSamples;
	std::vector<Viewpoint> viewpointBuffer(numPixels);

	std::cerr << "numPixels = " << numPixels << std::endl;
	std::cerr << "ray tracing work start" << std::endl;

	RayTracingWork rayTracingWork(m_scene, m_camera, m_sg->genCopied(1), viewpointBuffer.data(), numPixels, m_kernelRadius);
	rayTracingWork.doWork();

	std::cerr << "ray tracing work finished" << std::endl;
	std::cerr << "size of viewpoint buffer: " << sizeof(Viewpoint) * numPixels / 1024 / 1024 << " MB" << std::endl;

	std::size_t numPhotons = m_numPhotons;
	std::vector<Photon> photonBuffer(numPhotons);

	std::cerr << "numPhotons = " << numPhotons << std::endl;
	std::cerr << "photon mapping work start" << std::endl;

	std::size_t numEmittedPhotons;
	PhotonMappingWork photonMappingWork(m_scene, m_camera, m_sg->genCopied(1), photonBuffer.data(), numPhotons, &numEmittedPhotons);
	photonMappingWork.doWork();

	std::cerr << "photon mapping work finished" << std::endl;
	std::cerr << "size of photon buffer: " << sizeof(Photon) * numPhotons / 1024 / 1024 << " MB" << std::endl;

	std::cerr << "building photon map" << std::endl;

	PhotonMap photonMap(2, PhotonCenterCalculator());
	photonMap.build(std::move(photonBuffer));

	std::cerr << "photon map built" << std::endl;

	std::cerr << "estimating radiance" << std::endl;

	RadianceEstimateWork radianceEstimator(&photonMap, viewpointBuffer.data(), viewpointBuffer.size(), m_film.get(), numEmittedPhotons);
	radianceEstimator.doWork();

	std::cerr << "estimation complete" << std::endl;*/

	// DEBUG
	/*for(std::size_t y = 0; y < getRenderHeightPx(); ++y)
	{
		for(std::size_t x = 0; x < getRenderWidthPx(); ++x)
		{
			Viewpoint viewpoint = viewpointBuffer[y * getRenderWidthPx() + x];
			real filmXPx = viewpoint.filmNdcPos.x * static_cast<real>(getRenderWidthPx());
			real filmYPx = viewpoint.filmNdcPos.y * static_cast<real>(getRenderHeightPx());
			m_film->addSample(filmXPx, filmYPx, viewpointBuffer[y * getRenderWidthPx() + x].hit.getShadingNormal());
		}
	}*/
}

void PMRenderer::doRender()
{
	if(m_mode == EPMMode::VANILLA)
	{
		logger.log("rendering mode: vanilla photon mapping");

		renderWithVanillaPM();
	}
	else if(m_mode == EPMMode::PROGRESSIVE)
	{
		logger.log("rendering mode: progressive photon mapping");

		renderWithProgressivePM();
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, "unsupported PM mode, renders nothing");
	}
}

void PMRenderer::renderWithVanillaPM()
{
	logger.log("target number of photons: " + std::to_string(m_numPhotons));
	logger.log("size of photon buffer: " + std::to_string(sizeof(VPMPhoton) * m_numPhotons / 1024 / 1024) + " MB");
	logger.log("start shooting photons...");

	std::vector<VPMPhoton>   photonBuffer(m_numPhotons);
	std::vector<std::size_t> numPhotonPaths(getNumWorkers(), 0);
	parallel_work(m_numPhotons, getNumWorkers(),
		[this, &photonBuffer, &numPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = m_sg->genCopied(1);

			TPhotonMappingWork<VPMPhoton> photonMappingWork(
				m_scene,
				m_camera,
				sampleGenerator.get(),
				&(photonBuffer[workStart]),
				workEnd - workStart,
				&(numPhotonPaths[workerIdx]));

			photonMappingWork.setPMStatistics(&m_statistics);

			photonMappingWork.work();
		});
	const std::size_t totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), std::size_t(0));

	logger.log("building photon map...");

	TPhotonMap<VPMPhoton> photonMap(2, TPhotonCenterCalculator<VPMPhoton>());
	photonMap.build(std::move(photonBuffer));

	logger.log("estimating radiance...");

	parallel_work(m_numPasses, getNumWorkers(),
		[this, &photonMap, totalPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto numPasses       = workEnd - workStart;
			auto sampleGenerator = m_sg->genCopied(numPasses);
			auto film            = std::make_unique<HdrRgbFilm>(
				getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

			VPMRadianceEvaluationWork radianceEstimator(
				&photonMap, 
				totalPhotonPaths,
				m_scene,
				m_camera,
				sampleGenerator.get(),
				film.get());

			radianceEstimator.setPMRenderer(this);
			radianceEstimator.setPMStatistics(&m_statistics);
			radianceEstimator.setKernelRadius(m_kernelRadius);

			radianceEstimator.work();
		});
}

void PMRenderer::renderWithProgressivePM()
{
	logger.log("start gathering viewpoints...");

	std::vector<PPMViewpoint> viewpoints;
	{
		auto viewpointSampleGenerator = m_sg->genCopied(m_numSamplesPerPixel);
		TViewpointGatheringWork<PPMViewpoint> viewpointWork(
			m_scene, 
			m_camera, 
			viewpointSampleGenerator.get(),
			getRenderWindowPx(),
			m_kernelRadius);
		viewpointWork.work();

		viewpoints = viewpointWork.claimViewpoints();
	}
	
	logger.log("size of viewpoint buffer: " + std::to_string(sizeof(PPMViewpoint) * viewpoints.size() / 1024 / 1024) + " MB");

	std::size_t numPhotonsPerPass = m_numPhotons;
	logger.log("number of photons per pass: " + std::to_string(numPhotonsPerPass));
	logger.log("size of photon buffer: " + std::to_string(sizeof(VPMPhoton) * numPhotonsPerPass / 1024 / 1024) + " MB");

	logger.log("start accumulating passes...");

	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths = 0;
	while(numFinishedPasses < m_numPasses)
	{
		auto resultFilm = std::make_unique<HdrRgbFilm>(
			getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

		std::vector<PPMPhoton> photonBuffer(numPhotonsPerPass);

		std::vector<std::size_t> numPhotonPaths(getNumWorkers(), 0);
		parallel_work(numPhotonsPerPass, getNumWorkers(),
			[this, &photonBuffer, &numPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				auto sampleGenerator = m_sg->genCopied(1);

				TPhotonMappingWork<PPMPhoton> photonMappingWork(
					m_scene,
					m_camera,
					sampleGenerator.get(),
					&(photonBuffer[workStart]),
					workEnd - workStart,
					&(numPhotonPaths[workerIdx]));

				photonMappingWork.setPMStatistics(&m_statistics);

				photonMappingWork.work();
			});

		totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), totalPhotonPaths);

		TPhotonMap<PPMPhoton> photonMap(2, TPhotonCenterCalculator<PPMPhoton>());
		photonMap.build(std::move(photonBuffer));

		parallel_work(viewpoints.size(), getNumWorkers(),
			[this, &photonMap, &viewpoints, &resultFilm, totalPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				auto numViewpoints   = workEnd - workStart;
				auto film            = std::make_unique<HdrRgbFilm>(
					getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

				PPMRadianceEvaluationWork radianceEstimator(
					&photonMap, 
					totalPhotonPaths,
					film.get(),
					&(viewpoints[workStart]),
					numViewpoints,
					m_scene);

				radianceEstimator.setPMStatistics(&m_statistics);

				radianceEstimator.work();

				{
					std::lock_guard<std::mutex> lock(m_filmMutex);// TODO: use another lock

					resultFilm->mergeWith(*film);
				}
			});

		{
			std::lock_guard<std::mutex> lock(m_filmMutex);

			m_film->clear();
			m_film->mergeWith(*resultFilm);
		}

		m_statistics.asyncIncrementNumPasses();
		++numFinishedPasses;
	}// end while more pass needed
}

ERegionStatus PMRenderer::asyncPollUpdatedRegion(Region* out_region)
{
	*out_region = getRenderWindowPx();
	return ERegionStatus::UPDATING;
}

RenderProgress PMRenderer::asyncQueryRenderProgress()
{
	return RenderProgress(0, 0, 0);
}

void PMRenderer::asyncDevelopRegion(HdrRgbFrame& out_frame, const Region& region, const EAttribute attribute)
{
	std::lock_guard<std::mutex> lock(m_filmMutex);

	m_film->develop(out_frame, region);
}

void PMRenderer::develop(HdrRgbFrame& out_frame, const EAttribute attribute)
{
	m_film->develop(out_frame);
}

AttributeTags PMRenderer::supportedAttributes() const
{
	AttributeTags supported;
	supported.tag(EAttribute::LIGHT_ENERGY);
	return supported;
}

std::string PMRenderer::renderStateName(const RenderState::EType type, const std::size_t index) const
{
	PH_ASSERT_LT(index, RenderState::numStates(type));

	if(type == RenderState::EType::INTEGER)
	{
		switch(index)
		{
		case 0: return "finished passes";
		case 1: return "traced photons";
		default: return "";
		}
	}
	else if(type == RenderState::EType::REAL)
	{
		switch(index)
		{
		case 0: return "photons/second";
		default: return "";
		}
	}
	else
	{
		return "";
	}
}

RenderState PMRenderer::asyncQueryRenderState()
{
	RenderState state;
	state.setIntegerState(0, m_statistics.asyncGetNumPasses());
	state.setIntegerState(1, m_statistics.asyncGetNumTracedPhotons());
	return state;
}

void PMRenderer::asyncMergeFilm(HdrRgbFilm& srcFilm)
{
	std::lock_guard<std::mutex> lock(m_filmMutex);

	m_film->mergeWith(srcFilm);
	srcFilm.clear();
}

// command interface

PMRenderer::PMRenderer(const InputPacket& packet) : 
	Renderer(packet),
	m_film(),
	m_scene(nullptr),
	m_camera(nullptr),
	m_sg(nullptr),
	m_filter(SampleFilterFactory::createBlackmanHarrisFilter()),

	m_mode(),
	m_numPhotons(),
	m_kernelRadius(),
	m_numPasses(),
	m_numSamplesPerPixel(),

	m_filmMutex(),
	m_statistics()
{
	const std::string& mode = packet.getString("mode", "vanilla");
	if(mode == "vanilla")
	{
		m_mode = EPMMode::VANILLA;
	}
	else if(mode == "progressive")
	{
		m_mode = EPMMode::PROGRESSIVE;
	}
	else if(mode == "stochastic-progressive")
	{
		m_mode = EPMMode::STOCHASTIC_PROGRESSIVE;
	}

	m_numPhotons = packet.getInteger("num-photons", 100000);
	m_kernelRadius = packet.getReal("radius", 0.1_r);
	m_numPasses = packet.getInteger("num-passes", 1);
	m_numSamplesPerPixel = packet.getInteger("num-samples-per-pixel", 4);
}

SdlTypeInfo PMRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "pm");
}

void PMRenderer::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<PMRenderer>(packet);
	}));
}

}// end namespace ph