#pragma once

#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"

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

	/*! @brief Select target elemental to trace with `EViewPathSampleMode::SinglePath`.
	@param elemental The desired elemental for tracing the path.
	*/
	ViewPathTracingPolicy& traceSinglePathFor(SurfaceElemental elemental);

	/*! @brief Select target phenomena to trace with `EViewPathSampleMode::SinglePath`.
	@param phenomena The desired phenomena for tracing the path.
	*/
	ViewPathTracingPolicy& traceSinglePathFor(SurfacePhenomena phenomena);

	/*! @brief Select target phenomena to trace with `EViewPathSampleMode::ElementalBranch`.
	@param phenomena The target phenomena for tracing. Path will not be traced if the elemental's
	phenomenon is not one of the targets.
	*/
	ViewPathTracingPolicy& traceBranchedPathFor(SurfacePhenomena phenomena);

	bool isKilled() const;
	bool useRussianRoulette() const;
	bool hasTargetElemental() const;
	EViewPathSampleMode getSampleMode() const;
	SurfacePhenomena getTargetPhenomena() const;
	SurfaceElemental getTargetElemental() const;

private:
	bool                m_isKilled;
	bool                m_useRussianRoulette;
	bool                m_hasTargetElemental;
	EViewPathSampleMode m_sampleMode;
	SurfaceElemental    m_targetElemental;
	SurfacePhenomena    m_targetPhenomena;
};

// In-header Implementations:

inline ViewPathTracingPolicy::ViewPathTracingPolicy() : 
	m_isKilled(false),
	m_useRussianRoulette(true),
	m_hasTargetElemental(true),
	m_sampleMode(EViewPathSampleMode::SinglePath),
	m_targetElemental(ALL_SURFACE_ELEMENTALS),
	m_targetPhenomena(ALL_SURFACE_PHENOMENA)
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
	m_sampleMode         = EViewPathSampleMode::SinglePath;
	m_hasTargetElemental = true;
	m_targetElemental    = elemental;
	m_targetPhenomena    = ALL_SURFACE_PHENOMENA;

	return *this;
}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::traceSinglePathFor(const SurfacePhenomena phenomena)
{
	m_sampleMode         = EViewPathSampleMode::SinglePath;
	m_hasTargetElemental = true;
	m_targetPhenomena    = phenomena;
	m_targetElemental    = ALL_SURFACE_ELEMENTALS;

	return *this;
}

inline ViewPathTracingPolicy& ViewPathTracingPolicy::traceBranchedPathFor(const SurfacePhenomena phenomena)
{
	m_sampleMode         = EViewPathSampleMode::ElementalBranch;
	m_hasTargetElemental = false;
	m_targetPhenomena    = phenomena;
	m_targetElemental    = ALL_SURFACE_ELEMENTALS;

	return *this;
}

inline bool ViewPathTracingPolicy::useRussianRoulette() const
{
	return m_useRussianRoulette;
}

inline bool ViewPathTracingPolicy::hasTargetElemental() const
{
	return m_hasTargetElemental;
}

inline EViewPathSampleMode ViewPathTracingPolicy::getSampleMode() const
{
	return m_sampleMode;
}

inline SurfaceElemental ViewPathTracingPolicy::getTargetElemental() const
{
	PH_ASSERT(m_hasTargetElemental);
	PH_ASSERT(m_sampleMode == EViewPathSampleMode::SinglePath);

	return m_targetElemental;
}

inline SurfacePhenomena ViewPathTracingPolicy::getTargetPhenomena() const
{
	PH_ASSERT(!m_hasTargetElemental);
	PH_ASSERT(
		m_sampleMode == EViewPathSampleMode::SinglePath ||
		m_sampleMode == EViewPathSampleMode::ElementalBranch);

	return m_targetPhenomena;
}

inline bool ViewPathTracingPolicy::isKilled() const
{
	return m_isKilled;
}

}// end namespace ph
