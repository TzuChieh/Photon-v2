#include "DataIO/PlyFile.h"
#include "Common/logging.h"
#include "DataIO/Stream/BinaryFileInputStream.h"
#include "DataIO/io_exceptions.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PlyFile, DataIO);

namespace
{



}// end anonymous namespace

PlyFile::PlyProperty::PlyProperty() :
	name         (),
	dataType     (EPlyDataType::UNSPECIFIED),
	listCountType(EPlyDataType::UNSPECIFIED)
{}

bool PlyFile::PlyProperty::isList() const
{
	return listCountType != EPlyDataType::UNSPECIFIED;
}

PlyFile::PlyElement::PlyElement() :
	name       (),
	numElements(0),
	properties (),
	rawBuffer  ()
{}

PlyFile::PlyFile() :
	m_format  (EPlyFileFormat::ASCII),
	m_comments(),
	m_elements()
{}

PlyFile::PlyFile(const Path& plyFilePath) :
	PlyFile(plyFilePath, PlyIOConfig())
{}

PlyFile::PlyFile(const Path& plyFilePath, const PlyIOConfig& config) :
	PlyFile()
{

}

void PlyFile::setFormat(const EPlyFileFormat format)
{
	m_format = format;
}

void PlyFile::clear()
{
	m_comments.clear();
	m_elements.clear();
}

void PlyFile::readHeader(BinaryFileInputStream& stream)
{

}

}// end namespace ph
