#pragma once

#include "DataIO/BinaryFileReader.h"
#include "DataIO/FileSystem/Path.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <vector>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(TableFGD, BSDF);

class TableFGD final
{
public:
	explicit TableFGD(const Path& tableFilePath);

	real sample(
		real cosWi,
		real alpha, 
		real iorN, 
		real iorK) const;

	math::Spectrum sample(
		real cosWi,
		real alpha, 
		const math::Spectrum& iorN,
		const math::Spectrum& iorK) const;

private:
	std::vector<float> m_table;

	int m_numCosWi;
	int m_numAlpha;
	int m_numIorN;
	int m_numIorK;

	float m_minCosWi, m_maxCosWi;
	float m_minAlpha, m_maxAlpha;
	float m_minIorN,  m_maxIorN;
	float m_minIorK,  m_maxIorK;

	int calcIndex(int iCosWi, int iAlpha, int iIorN, int iIorK) const;
	void downSampleHalf();
};

// In-header Implementations:

inline TableFGD::TableFGD(const Path& tableFilePath) :
	m_table(),

	m_numCosWi(0),
	m_numAlpha(0),
	m_numIorN (0),
	m_numIorK (0),

	m_minCosWi(0.0f), m_maxCosWi(0.0f),
	m_minAlpha(0.0f), m_maxAlpha(0.0f),
	m_minIorN (0.0f), m_maxIorN (0.0f),
	m_minIorK (0.0f), m_maxIorK (0.0f)
{
	PH_LOG(TableFGD, Note, "loading <{}>", tableFilePath.toString());

	BinaryFileReader reader(tableFilePath);
	if(!reader.open())
	{
		return;
	}

	reader.read(&m_numCosWi);
	reader.read(&m_numAlpha);
	reader.read(&m_numIorN);
	reader.read(&m_numIorK);
	reader.read(&m_minCosWi); reader.read(&m_maxCosWi);
	reader.read(&m_minAlpha); reader.read(&m_maxAlpha);
	reader.read(&m_minIorN);  reader.read(&m_maxIorN);
	reader.read(&m_minIorK);  reader.read(&m_maxIorK);

	PH_DEBUG_LOG(TableFGD,
		"dimension: (cos-w_i = {}, alpha = {}, IOR-n = {}, IOR-k = {})", 
		m_numCosWi, m_numAlpha, m_numIorN, m_numIorK);

	PH_DEBUG_LOG(TableFGD, 
		"range: (cos-w_i = [{}, {}], alpha = [{}, {}], IOR-n = [{}, {}], IOR-k = [{}, {}])", 
		m_minCosWi, m_maxCosWi, 
		m_minAlpha, m_maxAlpha, 
		m_minIorN, m_maxIorN, 
		m_minIorK, m_maxIorK);

	PH_ASSERT(m_numCosWi > 0 && m_numAlpha > 0 && m_numIorN > 0 && m_numIorK > 0);

	const std::size_t tableSize = 
		static_cast<std::size_t>(m_numCosWi) *
		static_cast<std::size_t>(m_numAlpha) *
		static_cast<std::size_t>(m_numIorN) *
		static_cast<std::size_t>(m_numIorK);
	m_table.resize(tableSize, 0.0f);
	reader.read(m_table.data(), m_table.size());

	// TEST
	/*downSampleHalf();
	downSampleHalf();
	downSampleHalf();*/
}

inline math::Spectrum TableFGD::sample(
	const real            cosWi,
	const real            alpha,
	const math::Spectrum& iorN,
	const math::Spectrum& iorK) const
{
	math::Spectrum result;
	for(std::size_t i = 0; i < math::Spectrum::NUM_VALUES; ++i)
	{
		result[i] = sample(cosWi, alpha, iorN[i], iorK[i]);
	}
	return result;
}

inline int TableFGD::calcIndex(const int iCosWi, const int iAlpha, const int iIorN, const int iIorK) const
{
	// make sure the indices stay in the limits
	PH_ASSERT(0 <= iCosWi && iCosWi < m_numCosWi);
	PH_ASSERT(0 <= iAlpha && iAlpha < m_numAlpha);
	PH_ASSERT(0 <= iIorN  && iIorN  < m_numIorN);
	PH_ASSERT(0 <= iIorK  && iIorK  < m_numIorK);

	return iIorK + m_numIorK * (iIorN + m_numIorN * (iAlpha + m_numAlpha * iCosWi));
}

}// end namespace ph
