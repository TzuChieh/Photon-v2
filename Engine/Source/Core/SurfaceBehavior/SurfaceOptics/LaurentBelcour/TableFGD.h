#pragma once

#include "FileIO/BinaryFileReader.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"

#include <vector>

namespace ph
{

class TableFGD final
{
public:
	explicit TableFGD(const Path& tableFilePath);

	real sample(real sinWi, real alpha, real iorN, real iorK) const;

private:
	std::vector<float> m_table;

	int m_numSinWi;
	int m_numAlpha;
	int m_numIorN;
	int m_numIorK;

	float m_minSinWi, m_maxSinWi;
	float m_minAlpha, m_maxAlpha;
	float m_minIorN,  m_maxIorN;
	float m_minIorK,  m_maxIorK;

	static const Logger logger;
};

// In-header Implementations:

inline TableFGD::TableFGD(const Path& tableFilePath) :
	m_table(),

	m_numSinWi(0),
	m_numAlpha(0),
	m_numIorN (0),
	m_numIorK (0),

	m_minSinWi(0.0f), m_maxSinWi(0.0f),
	m_minAlpha(0.0f), m_maxAlpha(0.0f),
	m_minIorN (0.0f), m_maxIorN (0.0f),
	m_minIorK (0.0f), m_maxIorK (0.0f)
{
	logger.log(ELogLevel::NOTE_MED, "loading <" + tableFilePath.toString() + ">");

	BinaryFileReader reader(tableFilePath);
	if(!reader.open())
	{
		return;
	}

	reader.read(&m_numSinWi);
	reader.read(&m_numAlpha);
	reader.read(&m_numIorN);
	reader.read(&m_numIorK);
	reader.read(&m_minSinWi); reader.read(&m_maxSinWi);
	reader.read(&m_minAlpha); reader.read(&m_maxAlpha);
	reader.read(&m_minIorN);  reader.read(&m_maxIorN);
	reader.read(&m_minIorK);  reader.read(&m_maxIorK);

	logger.log(ELogLevel::DEBUG_MED, "dimension: "
		"(sin-w_i = " + std::to_string(m_numSinWi) + ", "
		 "alpha = " +   std::to_string(m_numAlpha) + ", "
		 "IOR-n = " +   std::to_string(m_numIorN) +  ", "
		 "IOR-k = " +   std::to_string(m_numIorK) + ")");
	logger.log(ELogLevel::DEBUG_MED, "range: "
		"(sin-w_i = [" + std::to_string(m_minSinWi) + ", " + std::to_string(m_maxSinWi) + "], "
		 "alpha = [" +   std::to_string(m_minAlpha) + ", " + std::to_string(m_maxAlpha) + "], "
		 "IOR-n = [" +   std::to_string(m_minIorN) + ", " +  std::to_string(m_maxIorN) +  "], "
		 "IOR-k = [" +   std::to_string(m_minIorK) + ", " +  std::to_string(m_maxIorK) +  "])");

	PH_ASSERT(m_numSinWi > 0 && m_numAlpha > 0 && m_numIorN > 0 && m_numIorK > 0);

	const std::size_t tableSize = 
		static_cast<std::size_t>(m_numSinWi) *
		static_cast<std::size_t>(m_numAlpha) *
		static_cast<std::size_t>(m_numIorN) *
		static_cast<std::size_t>(m_numIorK);
	m_table.resize(tableSize, 0.0f);
	reader.read(m_table.data(), m_table.size());
}

}// end namespace ph