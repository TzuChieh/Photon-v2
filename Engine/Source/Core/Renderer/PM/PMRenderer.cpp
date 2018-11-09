#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "FileIO/SDL/SdlResourcePack.h"
#include "Core/Renderer/PM/RayTracingWork.h"
#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/VPMRadianceEvaluationWork.h"
#include "Core/Renderer/PM/VPMPhoton.h"
#include "FileIO/SDL/InputPacket.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Utility/concurrent.h"

#include <numeric>

namespace ph
{

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
	renderWithVanillaPM();
}

void PMRenderer::renderWithVanillaPM()
{
	std::size_t numPixels = getRenderWindowPx().calcArea() * m_perPixelSamples;
	std::vector<Viewpoint> viewpointBuffer(numPixels);


	std::cerr << "ray tracing work finished" << std::endl;
	std::cerr << "size of viewpoint buffer: " << sizeof(Viewpoint) * numPixels / 1024 / 1024 << " MB" << std::endl;

	std::size_t numPhotons = m_numPhotons;
	std::vector<VPMPhoton> photonBuffer(numPhotons);

	std::cerr << "numPhotons = " << numPhotons << std::endl;
	std::cerr << "photon mapping work start" << std::endl;

	std::vector<std::size_t> numPhotonPaths(getNumWorkers(), 0);
	std::size_t bufferOffset = 0;

	parallel_work(numPhotons, getNumWorkers(), 
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

	std::size_t totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), std::size_t(0));
	

	std::cerr << "photon mapping work finished" << std::endl;
	std::cerr << "size of photon buffer: " << sizeof(VPMPhoton) * numPhotons / 1024 / 1024 << " MB" << std::endl;

	std::cerr << "building photon map" << std::endl;

	TPhotonMap<VPMPhoton> photonMap(2, TPhotonCenterCalculator<VPMPhoton>());
	photonMap.build(std::move(photonBuffer));

	std::cerr << "photon map built" << std::endl;

	std::cerr << "estimating radiance" << std::endl;

	parallel_work(getNumWorkers(), getNumWorkers(), 
		[this, &photonMap, &viewpointBuffer, totalPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = m_sg->genCopied(9999999);// FIXME
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

	std::cerr << "estimation complete" << std::endl;
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
		//case 0: return "samples/second";
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
	m_perPixelSamples(),

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
	m_perPixelSamples = packet.getInteger("per-pixel-samples", 1);
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