#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Frame/TFrame.h"

namespace ph
{

/*! @brief Writer for Portable FloatMap image format.

Reference site 1: http://www.pauldebevec.com/Research/HDR/PFM/
Reference site 2: http://netpbm.sourceforge.net/doc/pfm.html
*/
class PfmFileWriter final
{
public:
	explicit PfmFileWriter(const Path& filePath);

	bool save(const HdrRgbFrame& frame);

private:
	Path m_filePath;
};

}// end namespace ph
