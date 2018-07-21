#pragma once

#include "FileIO/BinaryFileReader.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"
#include "Common/assertion.h"

#include <vector>

namespace ph
{

class TableTIR final
{
public:
	explicit TableTIR(const Path& tableFilePath);

private:
	std::vector<float> m_table;

	int m_numProjectedWt;
	int m_numAlpha;
	int m_numIOR;

	float m_minProjectedWt, m_maxProjectedWt;
	float m_minAlpha,       m_maxAlpha;
	float m_minIOR,         m_maxIOR;

	static const Logger logger;
};

// In-header Implementations:

inline TableTIR::TableTIR(const Path& tableFilePath) :
	m_table(),

	m_numProjectedWt(0), 
	m_numAlpha      (0),
	m_numIOR        (0),

	m_minProjectedWt(0.0f), m_maxProjectedWt(0.0f),
	m_minAlpha      (0.0f), m_maxAlpha      (0.0f),
	m_minIOR        (0.0f), m_maxIOR        (0.0f)
{
	logger.log(ELogLevel::NOTE_MED, "loading <" + tableFilePath.toString() + ">");

	BinaryFileReader reader(tableFilePath);
	if(!reader.open())
	{
		return;
	}

	reader.read(&m_numProjectedWt);
	reader.read(&m_numAlpha);
	reader.read(&m_numIOR);
	reader.read(&m_minProjectedWt); reader.read(&m_maxProjectedWt);
	reader.read(&m_minAlpha);       reader.read(&m_maxAlpha);
	reader.read(&m_minIOR);         reader.read(&m_maxIOR);

	logger.log(ELogLevel::DEBUG_MED, "dimension: "
		"(projected-w_t = " + std::to_string(m_numProjectedWt) + ", "
		 "alpha = " +         std::to_string(m_numAlpha) +       ", "
		 "IOR = " +           std::to_string(m_numIOR) + ")");
	logger.log(ELogLevel::DEBUG_MED, "range: "
		"(projected-w_t = [" + std::to_string(m_minProjectedWt) + ", " + std::to_string(m_maxProjectedWt) + "], "
		 "alpha = [" +         std::to_string(m_minAlpha) +       ", " + std::to_string(m_maxAlpha) +       "], "
		 "IOR = [" +           std::to_string(m_minIOR) + ", " +         std::to_string(m_maxIOR) +         "])");

	PH_ASSERT(m_numProjectedWt > 0 && m_numAlpha > 0 && m_numIOR > 0);

	const std::size_t tableSize = 
		static_cast<std::size_t>(m_numProjectedWt) * 
		static_cast<std::size_t>(m_numAlpha) *
		static_cast<std::size_t>(m_numIOR);
	m_table.resize(tableSize, 0.0f);
	reader.read(m_table.data(), m_table.size());
}

}// end namespace ph