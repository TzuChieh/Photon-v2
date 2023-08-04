#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"
#include "Common/logging.h"

#include <cwchar>
#include <algorithm>

namespace ph
{

Path Path::toAbsolute() const
{
	return Path(std_filesystem::absolute(m_path));
}

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

std::size_t Path::toString(
	TSpan<char> out_buffer,
	std::size_t* const out_numTotalChars,
	const bool isNullTerminated) const
{
	PH_ASSERT(!out_buffer.empty());

	const wchar_t* wstr = m_path.native().c_str();
	std::mbstate_t state{};

	if(out_numTotalChars)
	{
		*out_numTotalChars = std::wcsrtombs(nullptr, &wstr, 0, &state) + isNullTerminated;
	}

	// `numCopiedChars` does not count null-terminator here
	auto numCopiedChars = std::wcsrtombs(out_buffer.data(), &wstr, out_buffer.size(), &state);

	// Possibly always make the result null-terminated
	if(isNullTerminated)
	{
		if(numCopiedChars == out_buffer.size())
		{
			out_buffer.back() = '\0';
		}
		else
		{
			out_buffer[numCopiedChars] = '\0';
			++numCopiedChars;
		}
	}

	return numCopiedChars;
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
