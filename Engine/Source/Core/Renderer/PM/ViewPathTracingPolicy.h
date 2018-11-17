#pragma once

#include "Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Common/assertion.h"

namespace ph
{

/*
	Directives on how to trace the next path.
*/
enum class EViewPathSampleMode
{
	// Keep tracing current path.
	SINGLE_PATH,

	// Trace a different path for each elemental.
	ELEMENTAL_BRANCH
};

/*
	Specifying and querying policies for tracing view path.
*/
class ViewPathTracingPolicy
{
public:
	ViewPathTracingPolicy();

	// Kills the path.
	ViewPathTracingPolicy& kill();

	// Use russian roulette to kill the path.
	ViewPathTracingPolicy& useRussianRoulette(bool useRR);

	// Effectively as specifying EViewPathSampleMode::SINGLE_PATH. It is also
	// possible to specify the desired elemental for tracing the path.
	ViewPathTracingPolicy& traceSinglePathFor(SurfaceElemental elemental);

	// Effectively as specifying EViewPathSampleMode::ELEMENTAL_BRANCH. It is 
	// also possible to select target phenomena for tracing. Path will not be
	// traced if the elemental's phenomenon is not one of the targets.
	ViewPathTracingPolicy& traceBranchedPathFor(SurfacePhenomena phenomena);

	bool isKilled() const;
	bool useRussianRoulette() const;
	EViewPathSampleMode getSampleMode() const;
	SurfacePhenomena getTargetPhenomena() const;
	SurfaceElemental getTargetElemental() const;

private:
	bool                m_isKilled;
	bool                m_useRussianRoulette;
	EViewPathSampleMode m_sampleMode;
	SurfaceElemental    m_targetElemental;
	SurfacePhenomena    m_targetPhenomena;
};

// In-header Implementations:

inline ViewPathTracingPolicy::ViewPathTracingPolicy() : 
	m_isKilled(false),
	m_useRussianRoulette(true),
	m_sampleMode(EViewPathSampleMode::SINGLE_PATH),
	m_targetElemental(ALL_ELEMENTALS),
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

inline ViewPathTracingPolicy& ViewPathTracingPolicy::traceSinglePathFor(const SurfaceElemental elemental)
{
	m_sampleMode      = EViewPathSampleMode::SINGLE_PATH;
	m_targetElemental = elemental;

	return *this;
}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::traceBranchedPathFor(const SurfacePhenomena phenomena)
{
	m_sampleMode      = EViewPathSampleMode::ELEMENTAL_BRANCH;
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

inline SurfaceElemental ViewPathTracingPolicy::getTargetElemental() const
{
	PH_ASSERT(m_sampleMode == EViewPathSampleMode::SINGLE_PATH);

	return m_targetElemental;
}

inline SurfacePhenomena ViewPathTracingPolicy::getTargetPhenomena() const
{
	PH_ASSERT(m_sampleMode == EViewPathSampleMode::ELEMENTAL_BRANCH);

	return m_targetPhenomena;
}

inline bool ViewPathTracingPolicy::isKilled() const
{
	return m_isKilled;
}

}// end namespace ph