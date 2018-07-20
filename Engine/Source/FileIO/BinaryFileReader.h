#pragma once

#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"
#include "Common/assertion.h"

#include <fstream>
#include <vector>

namespace ph
{

class BinaryFileReader
{
public:
	explicit BinaryFileReader(const Path& path);
	virtual ~BinaryFileReader();

	bool open();
	void close();

	template<typename T>
	void read(T* out_buffer, std::size_t numElements = 1);

private:
	Path          m_path;
	std::ifstream m_inputStream;

	static const Logger logger;
};

// In-header Implementations:

inline BinaryFileReader::BinaryFileReader(const Path& path) :
	m_path(path), m_inputStream()
{}

inline BinaryFileReader::~BinaryFileReader()
{
	close();
}

inline bool BinaryFileReader::open()
{
	m_inputStream.open(m_path.toString(), std::ios_base::in | std::ios_base::binary);
	if(!m_inputStream.good())
	{
		logger.log(ELogLevel::WARNING_MED, "file <" + m_path.toString() + "> open failed");
		return false;
	}

	return true;
}

inline void BinaryFileReader::close()
{
	m_inputStream.close();
}

template<typename T>
void BinaryFileReader::read(T* const out_buffer, const std::size_t numElements)
{
	PH_ASSERT(out_buffer && numElements > 0 && m_inputStream.good());
	
	m_inputStream.read(reinterpret_cast<char*>(out_buffer), sizeof(T) * numElements);
}

}// end namespace ph