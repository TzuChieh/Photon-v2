#pragma once

#include "Utility/string_utils.h"

#include <string>
#include <vector>
#include <optional>
#include <type_traits>

namespace ph
{

/*! @brief Helper for parsing command line arguments.
*/
class CommandLineArguments final
{
public:
	CommandLineArguments(int argc, char* argv[]);

	/*! @brief Get the program name.
	@return An empty string if program name is not available.
	*/
	std::string getProgramName() const;

	/*! @brief Check if there are arguments yet to be parsed.
	*/
	bool isEmpty() const;

	/*! @brief Get the first argument passed in and remove it from the internal buffer.
	@param defaultValue A default value if the operation cannot be done (such as isEmpty() is true).
	*/
	std::string retrieveString(const std::string& defaultString = "");

	/*! @brief Get the first N arguments passed in and remove them from the internal buffer.
	@param numValues Number of values to be retrieved at once.
	*/
	std::vector<std::string> retrieveStrings(std::size_t numValues);

	/*! @brief Get the arguments for an option.
	This method assumes that the options specified are of the form 
	"{- | --}<optionName> <arg0> <arg1> ...", i.e., options have a single or
	double dash prefix followed by its name, then the actual arguments. Careful
	that some input forms may still require manual treatment (using retrieve())
	such as a filename starting with a dash or a negative number, since they can
	be misinterpreted as the next option and cause the argument list for the
	current option being ended prematurely.
	@param optionPrefix The option's prefix.
	*/
	std::vector<std::string> retrieveOptionArguments(const std::string& optionPrefix);

	/*! @brief Get the arguments between a specified range.
	@param startingPrefix The first argument's prefix.
	@param endingPrefix The last argument's prefix.
	@param shouldIncludeStart Whether to include the first matching argument.
	@param shouldIncludeEnd Whether to include the last matching argument.
	*/
	std::vector<std::string> retrieveStrings(
		const std::string& startingPrefix, 
		const std::string& endingPrefix, 
		bool shouldIncludeStart = true,
		bool shouldIncludeEnd = true);

	/*! @brief Get an integer from the arguments.
	Similar to retrieveString(const std::string&), while the result is converted to an integer.
	*/
	template<typename T>
	T retrieveInt(T defaultInt = 0);

	/*! @brief Get a float from the arguments.
	Similar to retrieveString(const std::string&), while the result is converted to a float.
	*/
	template<typename T>
	T retrieveFloat(T defaultFloat = 0.0f);

	template<typename T>
	std::optional<T> retrieve();

private:
	std::string              m_programName;
	std::vector<std::string> m_arguments;
};

// In-header Implementations:

inline std::string CommandLineArguments::getProgramName() const
{
	return m_programName;
}

inline bool CommandLineArguments::isEmpty() const
{
	return m_arguments.empty();
}

template<typename T>
inline T CommandLineArguments::retrieveInt(T defaultInt)
{
	static_assert(std::is_integral_v<T>,
		"expect argument type to be integer");

	auto optInt = retrieve<T>();
	return optInt ? *optInt : defaultInt;
}

template<typename T>
inline T CommandLineArguments::retrieveFloat(T defaultFloat)
{
	static_assert(std::is_floating_point_v<T>,
		"expect argument type to be floating-point");

	auto optFloat = retrieve<T>();
	return optFloat ? *optFloat : defaultFloat;
}

template<typename T>
inline std::optional<T> CommandLineArguments::retrieve()
{
	if(isEmpty())
	{
		return std::nullopt;
	}

	std::string argument = m_arguments.front();
	m_arguments.erase(m_arguments.begin());

	if constexpr(std::is_same_v<T, std::string>)
	{
		return argument;
	}
	else
	{
		static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
			"expect argument type to be integer or floating-point");

		return string_utils::parse_number<T>(argument);
	}
}

}// end namespace ph
