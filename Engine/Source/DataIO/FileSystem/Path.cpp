#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"
#include "Common/logging.h"

#include <cwchar>
#include <algorithm>

namespace ph
{

Path::Path()
	: m_path()
{}

Path::Path(std::string path)
	: m_path(std::move(path), std::filesystem::path::generic_format)
{}

Path::Path(std::string_view path)
	: m_path(path, std::filesystem::path::generic_format)
{}

Path::Path(TSpanView<char> path)
	: m_path(path.begin(), path.end(), std::filesystem::path::generic_format)
{}

Path::Path(const char* path)
	: m_path(path, std::filesystem::path::generic_format)
{}

Path::Path(std::filesystem::path path)
	: m_path(std::move(path))
{}

Path Path::append(const Path& other) const
{
	Path thisPath = this->removeTrailingSeparator();
	Path otherPath = other.removeLeadingSeparator();
	return Path(thisPath.m_path / otherPath.m_path);
}

Path Path::append(std::string_view pathStr) const
{
	return append(Path(pathStr));
}

Path Path::toAbsolute() const
{
	return Path(std::filesystem::absolute(m_path));
}

std::string Path::toString() const
{
	return m_path.generic_string();
}

std::string Path::toAbsoluteString() const
{
	return std::filesystem::absolute(m_path).generic_string();
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

Path Path::removeLeadingSeparator() const
{
	std::string pathStr = m_path.generic_string();
	while(!pathStr.empty())
	{
		//if(charToWchar(pathStr.front()) == m_path.preferred_separator)
		if(pathStr.front() == '/')
		{
			pathStr.erase(pathStr.begin());
		}
		else
		{
			break;
		}
	}

	return Path(pathStr);
}

Path Path::removeTrailingSeparator() const
{
	std::string pathStr = m_path.generic_string();
	while(!pathStr.empty())
	{
		//if(charToWchar(pathStr.back()) == m_path.preferred_separator)
		if(pathStr.back() == '/')
		{
			pathStr.pop_back();
		}
		else
		{
			break;
		}
	}

	return Path(pathStr);
}

std::string Path::getFilename() const
{
	return m_path.filename().generic_string();
}

std::string Path::getExtension() const
{
	return m_path.extension().generic_string();
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
	return Path(std::filesystem::path(m_path).replace_extension(replacement));
}

Path Path::removeExtension() const
{
	return replaceExtension("");
}

std::string Path::toNativeString() const
{
	return m_path.string();
}

std::size_t Path::toNativeString(
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
