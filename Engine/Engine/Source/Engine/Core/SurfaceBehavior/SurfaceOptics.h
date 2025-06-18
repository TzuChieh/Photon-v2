#pragma once

#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Engine/Core/SurfaceBehavior/bsdf_query_fwd.h"

#include <string>
#include <array>

namespace ph
{

class SampleFlow;

/*! @brief Describes how light interacts with a surface. 
*/
class SurfaceOptics
{
public:
	SurfaceOptics();
	virtual ~SurfaceOptics() = default;

	/*! @brief Get the phenomenon of a surface component.
	One can also setup query for a specific elemental. See `BsdfQueryContext`.
	*/
	virtual ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const = 0;

	/*! @brief Calculate BSDF.
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void calcBsdfCore(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const = 0;

	/*! @brief Generate BSDF sample.
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void genBsdfSampleCore(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const = 0;

	/*! @brief Calculate BSDF PDF.
	Implementations do not need to care whether the input or output vectors are in a geometrically
	possible configuration (e.g., sidedness according to `ESidednessPolicy`).
	*/
	virtual void calcBsdfPdfCore(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const = 0;

	virtual std::string toString() const;

	/*! @brief Executes a BSDF evaluation query.
	Respects sidedness policy.
	*/
	void calcBsdf(BsdfEvalQuery& eval) const;

	/*! @brief Executes a BSDF sample query.
	Respects sidedness policy.
	*/
	void genBsdfSample(BsdfSampleQuery& sample, SampleFlow& sampleFlow) const;

	/*! @brief Executes a BSDF sample PDF query.
	Respects sidedness policy.
	*/
	void calcBsdfPdf(BsdfPdfQuery& pdfQuery) const;

	/*! @brief Get all phenomena that exist in this surface.
	@note If a surface has any delta phenomenon along with any non-delta phenomenon, it should be
	treated the same way as a surface with only pure delta phenomenon.
	*/
	SurfacePhenomena getAllPhenomena() const;

	/*! @brief Get number of components this surface contains.
	@see `getPhenomenonOf()`.
	*/
	SurfaceElemental numElementals() const;

protected:
	SurfacePhenomena m_phenomena;
	SurfaceElemental m_numElementals;
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
