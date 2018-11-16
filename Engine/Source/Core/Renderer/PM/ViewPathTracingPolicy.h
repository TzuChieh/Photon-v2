#pragma once

#include "Core/SurfaceBehavior/surface_optics_fwd.h"

namespace ph
{

enum class EViewPathSampleMode
{
	SINGLE_PATH,
	ELEMENTAL_BRANCH
};

class ViewPathTracingPolicy
{
public:
	ViewPathTracingPolicy();

	ViewPathTracingPolicy& kill();
	ViewPathTracingPolicy& useRussianRoulette(bool useRR);
	ViewPathTracingPolicy& traceSinglePath();
	ViewPathTracingPolicy& traceBranchedPathFor(SurfacePhenomena phenomena);

	bool isKilled() const;
	bool useRussianRoulette() const;
	EViewPathSampleMode getSampleMode() const;
	SurfacePhenomena getTargetPhenomena() const;

private:
	bool                m_isKilled;
	bool                m_useRussianRoulette;
	EViewPathSampleMode m_sampleMode;
	SurfacePhenomena    m_targetPhenomena;
};

// In-header Implementations:

inline ViewPathTracingPolicy::ViewPathTracingPolicy() : 
	m_isKilled(false),
	m_useRussianRoulette(true),
	m_sampleMode(EViewPathSampleMode::SINGLE_PATH),
	m_targetPhenomena()
{}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::kill()
{
	m_isKilled = true;

	return *this;
}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::useRussianRoulette(const bool useRR)
{
	m_useRussianRoulette = useRR;

	return *this;
}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::traceSinglePath()
{
	m_sampleMode = EViewPathSampleMode::SINGLE_PATH;

	return *this;
}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::traceBranchedPathFor(const SurfacePhenomena phenomena)
{
	m_targetPhenomena = phenomena;

	return *this;
}

inline bool ViewPathTracingPolicy::useRussianRoulette() const
{
	return m_useRussianRoulette;
}

inline EViewPathSampleMode ViewPathTracingPolicy::getSampleMode() const
{
	return m_sampleMode;
}

inline SurfacePhenomena ViewPathTracingPolicy::getTargetPhenomena() const
{
	return m_targetPhenomena;
}

inline bool ViewPathTracingPolicy::isKilled() const
{
	return m_isKilled;
}

}// end namespace ph