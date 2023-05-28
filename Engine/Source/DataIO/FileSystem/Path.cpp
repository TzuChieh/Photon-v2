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

bool Path::operator == (const Path& other) const
{
	return m_path == other.m_path;
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

Path Path::getParent() const
{
	return Path(m_path.parent_path());
}

Path Path::replaceExtension(std::string_view replacement) const
{
	return Path(std_filesystem::path(m_path).replace_extension(replacement));
}

Path Path::removeExtension() const
{
	return replaceExtension("");
}

}// end namespace ph
