#pragma once

#include "Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Core/SurfaceBehavior/bsdf_query_fwd.h"

#include <string>
#include <array>

namespace ph
{

class SampleFlow;

class SurfaceOptics
{
	// FIXME: sort-of hacked, should clarify the need of this in the future
	friend class LerpedSurfaceOptics;

public:
	SurfaceOptics();
	virtual ~SurfaceOptics();

	virtual ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const = 0;

	/*! @brief Executes a BSDF evaluation query.
	Respects sidedness policy.
	*/
	void calcBsdf(BsdfEvalQuery& eval) const;

	/*! @brief Executes a BSDF sample query.
	Respects sidedness policy.
	*/
	void calcBsdfSample(BsdfSampleQuery& sample, SampleFlow& sampleFlow) const;

	/*! @brief Executes a BSDF sample PDF query.
	Respects sidedness policy.
	*/
	void calcBsdfSamplePdfW(BsdfPdfQuery& pdfQuery) const;

	SurfacePhenomena getAllPhenomena() const;
	SurfaceElemental numElementals() const;

	virtual std::string toString() const;

protected:
	SurfacePhenomena m_phenomena;
	SurfaceElemental m_numElementals;

private:
	/*!
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const = 0;

	/*!
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void calcBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const = 0;

	/*!
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void calcBsdfSamplePdfW(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const = 0;
};

// In-header Implementations:

inline SurfacePhenomena SurfaceOptics::getAllPhenomena() const
{
	return m_phenomena;
}

inline SurfaceElemental SurfaceOptics::numElementals() const
{
	return m_numElementals;
}

inline std::string SurfaceOptics::toString() const
{
	return std::to_string(m_numElementals) + " elementals";
}

}// end namespace ph
