#include "Core/Renderer/PPM/PPMRenderer.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "FileIO/SDL/SdlResourcePack.h"
#include "Core/Renderer/PPM/RayTracingWork.h"
#include "Core/Renderer/PPM/PhotonMappingWork.h"
#include "Core/Renderer/PPM/PhotonMap.h"
#include "Core/Renderer/PPM/RadianceEstimateWork.h"

namespace ph
{

void PPMRenderer::doUpdate(const SdlResourcePack& data)
{
	m_film = std::make_unique<HdrRgbFilm>(getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

	m_scene = &(data.visualWorld.getScene());
	m_camera = data.getCamera().get();
	m_sg = data.getSampleGenerator().get();
	
	/*Scene* scene,
		Camera* camera,
		std::unique_ptr<SampleGenerator> sampleGenerator,
		Viewpoint* viewpointBuffer,
		std::size_t numViewpoints*/

	std::size_t numPixels = getRenderWindowPx().calcArea();
	std::vector<Viewpoint> viewpointBuffer(numPixels);

	std::cerr << "numPixels = " << numPixels << std::endl;
	std::cerr << "ray tracing work start" << std::endl;

	RayTracingWork rayTracingWork(m_scene, m_camera, m_sg->genCopied(1), viewpointBuffer.data(), numPixels);
	rayTracingWork.doWork();

	std::cerr << "ray tracing work finished" << std::endl;
	std::cerr << "size of viewpoint buffer: " << sizeof(Viewpoint) * numPixels / 1024 / 1024 << " MB" << std::endl;

	std::size_t numPhotons = 200000;
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

	std::cerr << "estimation complete" << std::endl;

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

void PPMRenderer::doRender()
{}

ERegionStatus PPMRenderer::asyncPollUpdatedRegion(Region* out_region)
{
	return ERegionStatus::INVALID;
}

RenderState PPMRenderer::asyncQueryRenderState()
{
	return RenderState();
}

RenderProgress PPMRenderer::asyncQueryRenderProgress()
{
	return RenderProgress(0, 0);
}

void PPMRenderer::asyncDevelopRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute)
{}

void PPMRenderer::develop(HdrRgbFrame& out_frame, EAttribute attribute)
{
	m_film->develop(out_frame);
}

AttributeTags PPMRenderer::supportedAttributes() const
{
	AttributeTags supported;
	supported.tag(EAttribute::LIGHT_ENERGY);
	return supported;
}

std::string PPMRenderer::renderStateName(const RenderState::EType type, const std::size_t index) const
{
	PH_ASSERT_LT(index, RenderState::numStates(type));

	if(type == RenderState::EType::INTEGER)
	{
		return "";
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

// command interface

PPMRenderer::PPMRenderer(const InputPacket& packet) : 
	Renderer(packet),
	m_film(),
	m_scene(nullptr),
	m_camera(nullptr),
	m_sg(nullptr),
	m_filter(SampleFilterFactory::createBlackmanHarrisFilter())
{}

SdlTypeInfo PPMRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "ppm");
}

void PPMRenderer::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<PPMRenderer>(packet);
	}));
}

}// end namespace ph