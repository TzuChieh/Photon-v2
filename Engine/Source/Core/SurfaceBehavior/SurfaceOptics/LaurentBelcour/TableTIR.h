#pragma once

#include "FileIO/BinaryFileReader.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"

#include <vector>

namespace ph
{

class TableTIR final
{
public:
	explicit TableTIR(const Path& tableFilePath);

	real sample(real sinWt, real alpha, real relIor) const;

private:
	std::vector<float> m_table;

	int m_numSinWt;
	int m_numAlpha;
	int m_numRelIor;

	float m_minSinWt,  m_maxSinWt;
	float m_minAlpha,  m_maxAlpha;
	float m_minRelIor, m_maxRelIor;

	static const Logger logger;
};

// In-header Implementations:

inline TableTIR::TableTIR(const Path& tableFilePath) :
	m_table(),

	m_numSinWt (0),
	m_numAlpha (0),
	m_numRelIor(0),

	m_minSinWt (0.0f), m_maxSinWt (0.0f),
	m_minAlpha (0.0f), m_maxAlpha (0.0f),
	m_minRelIor(0.0f), m_maxRelIor(0.0f)
{
	logger.log(ELogLevel::NOTE_MED, "loading <" + tableFilePath.toString() + ">");

	BinaryFileReader reader(tableFilePath);
	if(!reader.open())
	{
		return;
	}

	reader.read(&m_numSinWt);
	reader.read(&m_numAlpha);
	reader.read(&m_numRelIor);
	reader.read(&m_minSinWt);  reader.read(&m_maxSinWt);
	reader.read(&m_minAlpha);  reader.read(&m_maxAlpha);
	reader.read(&m_minRelIor); reader.read(&m_maxRelIor);

	logger.log(ELogLevel::DEBUG_MED, "dimension: "
		"(sin-w_t = " +      std::to_string(m_numSinWt) + ", "
		 "alpha = " +        std::to_string(m_numAlpha) + ", "
		 "relative-IOR = " + std::to_string(m_numRelIor) + ")");
	logger.log(ELogLevel::DEBUG_MED, "range: "
		"(sin-w_t = [" +      std::to_string(m_minSinWt) +  ", " + std::to_string(m_maxSinWt) +  "], "
		 "alpha = [" +        std::to_string(m_minAlpha) +  ", " + std::to_string(m_maxAlpha) +  "], "
		 "relative-IOR = [" + std::to_string(m_minRelIor) + ", " + std::to_string(m_maxRelIor) + "])");

	PH_ASSERT(m_numSinWt > 0 && m_numAlpha > 0 && m_numRelIor > 0);

	const std::size_t tableSize = 
		static_cast<std::size_t>(m_numSinWt) *
		static_cast<std::size_t>(m_numAlpha) *
		static_cast<std::size_t>(m_numRelIor);
	m_table.resize(tableSize, 0.0f);
	reader.read(m_table.data(), m_table.size());
}

}// end namespace ph