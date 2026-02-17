#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>

namespace ph
{

/*! @brief User specified information and requirements.
*/
class SdlUserSpec final
{
public:
	SdlUserSpec() = default;

	explicit SdlUserSpec(std::map<std::string, std::vector<std::string>> keyToArgs);

	bool hasKey(const std::string& key) const;
	bool hasArg(const std::string& key, const std::string& arg) const;

	/*! @brief Get positional argument (key without mapped values) at the specified index.
	@return The argument. Empty if index is out of range.
	*/
	std::string getArg(std::size_t argIdx) const;

	std::vector<std::string> getArgs(const std::string& key) const;

private:
	std::map<std::string, std::vector<std::string>> m_keyToArgs;
};

inline SdlUserSpec::SdlUserSpec(std::map<std::string, std::vector<std::string>> keyToArgs)
	: m_keyToArgs(std::move(keyToArgs))
{}

inline bool SdlUserSpec::hasKey(const std::string& key) const
{
	return m_keyToArgs.contains(key);
}

inline bool SdlUserSpec::hasArg(const std::string& key, const std::string& arg) const
{
	const auto findResult = m_keyToArgs.find(key);
	if(findResult == m_keyToArgs.end())
	{
		return false;
	}

	const auto& args = findResult->second;
	return std::find(args.begin(), args.end(), arg) != args.end();
}

inline std::string SdlUserSpec::getArg(const std::size_t argIdx) const
{
	if(argIdx >= m_keyToArgs.size())
	{
		return "";
	}

	auto iter = m_keyToArgs.begin();
	std::advance(iter, argIdx);
	return iter->first;
}

inline std::vector<std::string> SdlUserSpec::getArgs(const std::string& key) const
{
	return hasKey(key) ? m_keyToArgs.at(key) : std::vector<std::string>{};
}

}// end namespace ph
