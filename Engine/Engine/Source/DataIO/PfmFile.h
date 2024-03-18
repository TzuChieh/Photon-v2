#pragma once

#include "Frame/TFrame.h"

#include <Common/assertion.h>

namespace ph { class Path; }

namespace ph
{

/*! @brief Writer for Portable FloatMap image format.

Reference site 1: http://www.pauldebevec.com/Research/HDR/PFM/
Reference site 2: http://netpbm.sourceforge.net/doc/pfm.html
*/
class PfmFile final
{
public:
	PfmFile() = default;
	explicit PfmFile(const TFrame<float32, 3>& colorFrame);

	void copy(const TFrame<float32, 3>& colorFrame);
	void load(const Path& filePath);
	void save(const Path& filePath) const;
	const TFrame<float32, 3>& getColorFrame() const;
	bool isMonochromatic() const;

private:
	TFrame<float32, 3> m_frame;
	bool m_isMonochromatic = false;
};

inline PfmFile::PfmFile(const TFrame<float32, 3>& colorFrame)
	: m_frame(colorFrame)
	, m_isMonochromatic(false)
{}

inline void PfmFile::copy(const TFrame<float32, 3>& colorFrame)
{
	m_frame = colorFrame;
	m_isMonochromatic = false;
}

inline const TFrame<float32, 3>& PfmFile::getColorFrame() const
{
	PH_ASSERT(!m_isMonochromatic);

	return m_frame;
}

inline bool PfmFile::isMonochromatic() const
{
	return m_isMonochromatic;
}

}// end namespace ph
