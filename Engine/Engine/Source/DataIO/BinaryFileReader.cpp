#include "DataIO/BinaryFileReader.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BinaryFileReader, DataIO);

bool BinaryFileReader::open()
{
	m_inputStream.open(m_filePath.toString(), std::ios_base::in | std::ios_base::binary);
	if(!m_inputStream.good())
	{
		PH_LOG_WARNING(BinaryFileReader, "<{}> open failed", m_filePath.toString());
		return false;
	}

	return true;
}

}// end namespace ph
