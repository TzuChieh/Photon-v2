#include "DataIO/FileSystem/Path.h"

namespace ph
{

Path Path::operator / (const Path& other) const
{
	return Path(*this).append(other);
}

Path Path::operator / (std::string_view pathStr) const
{
	return Path(*this).append(pathStr);
}

Path Path::getLeadingElement() const
{
	return m_path.begin() != m_path.end()
		? Path(*m_path.begin())
		: Path();
}

Path Path::getTrailingElement(const bool ignoreTrailingSeparator) const
{
	if(m_path.begin() == m_path.end())
	{
		return Path();
	}

	auto lastIter = --m_path.end();
	if(ignoreTrailingSeparator && lastIter->empty())
	{
		lastIter = m_path.begin() != lastIter ? (--lastIter) : m_path.begin();
	}

	return Path(*lastIter);
}

}// end namespace ph
