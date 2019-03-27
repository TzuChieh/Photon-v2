#include "FileIO/ExrFileWriter.h"

namespace ph
{

ExrFileWriter::ExrFileWriter(const Path& filePath) : 
	m_filePath(filePath)
{}

bool ExrFileWriter::save(const HdrRgbFrame& frame)
{
	// TODO

	return false;
}

}// end namespace ph