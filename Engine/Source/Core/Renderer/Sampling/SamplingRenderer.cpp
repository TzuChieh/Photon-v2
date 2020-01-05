#include "Core/Renderer/Sampling/SamplingRenderer.h"
#include "Common/assertion.h"
#include "Common/Logger.h"
#include "DataIO/SDL/InputPacket.h"
#include "Core/Filmic/SampleFilters.h"
#include "Core/Estimator/BVPTEstimator.h"
#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Estimator/BVPTDLEstimator.h"

namespace ph
{

// command interface

SamplingRenderer::SamplingRenderer(const InputPacket& packet) :

	Renderer(packet),

	m_filter   (SampleFilters::createGaussianFilter()),
	m_estimator(nullptr)
{
	if(packet.hasString("filter-name"))
	{
		m_filter = SampleFilters::create(packet.getString("filter-name"));
	}

	const std::string estimatorName = packet.getString("estimator", "bneept");
	if(estimatorName == "bneept")
	{
		m_estimator = std::make_unique<BNEEPTEstimator>();
	}
	else if(estimatorName == "bvpt")
	{
		m_estimator = std::make_unique<BVPTEstimator>();
	}
	else if(estimatorName == "bvptdl")
	{
		m_estimator = std::make_unique<BVPTDLEstimator>();
	}

	PH_ASSERT(m_estimator);
}

SdlTypeInfo SamplingRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "sampling");
}

void SamplingRenderer::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
