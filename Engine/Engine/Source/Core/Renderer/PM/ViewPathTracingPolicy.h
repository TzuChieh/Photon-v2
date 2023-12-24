#pragma once

#include "Core/SurfaceBehavior/surface_optics_fwd.h"

#include <Common/assertion.h>

namespace ph
{

/*! @brief Directives on how to trace the next path.
*/
enum class EViewPathSampleMode
{
	/*! Keep tracing current path. */
	SinglePath,

	/*! Trace a different path for each elemental. */
	ElementalBranch
};

/*! @brief Specifying and querying policies for tracing view path.
*/
class ViewPathTracingPolicy
{
public:
	ViewPathTracingPolicy();

	/*! @brief Kills the path.
	*/
	ViewPathTracingPolicy& kill();

	/*! @brief Use russian roulette to kill the path.
	*/
	ViewPathTracingPolicy& useRussianRoulette(bool useRR);

	/*! @brief Effectively as specifying `EViewPathSampleMode::SinglePath`.
	@param elemental The desired elemental for tracing the path.
	*/
	ViewPathTracingPolicy& traceSinglePathFor(SurfaceElemental elemental);

	/*! @brief Effectively as specifying `EViewPathSampleMode::ElementalBranch`.
	@param phenomena The target phenomena for tracing. Path will not be traced if the elemental's
	phenomenon is not one of the targets.
	*/
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
	m_sampleMode(EViewPathSampleMode::SinglePath),
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
	m_sampleMode      = EViewPathSampleMode::SinglePath;
	m_targetElemental = elemental;

	return *this;
}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::traceBranchedPathFor(const SurfacePhenomena phenomena)
{
	m_sampleMode      = EViewPathSampleMode::ElementalBranch;
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
	PH_ASSERT(m_sampleMode == EViewPathSampleMode::SinglePath);

	return m_targetElemental;
}

inline SurfacePhenomena ViewPathTracingPolicy::getTargetPhenomena() const
{
	PH_ASSERT(m_sampleMode == EViewPathSampleMode::ElementalBranch);

	return m_targetPhenomena;
}

inline bool ViewPathTracingPolicy::isKilled() const
{
	return m_isKilled;
}

}// end namespace ph
