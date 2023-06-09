#include "DataIO/FileSystem/Path.h"
#include "Common/logging.h"

#include <cwchar>

namespace ph
{

std::string Path::toAbsoluteString() const
{
	return std_filesystem::absolute(m_path).string();
}

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

wchar_t Path::charToWchar(const char ch)
{
	const std::wint_t wch = std::btowc(ch);
	if(wch == WEOF)
	{
		PH_DEFAULT_LOG_WARNING(
			"at Path::charToWchar(), char <{}> failed to widen to wchar",
			ch);
	}

	return static_cast<wchar_t>(wch);
}

}// end namespace ph
