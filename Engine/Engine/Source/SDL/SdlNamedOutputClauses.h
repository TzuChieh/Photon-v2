#pragma once

#include "SDL/SdlOutputClauses.h"

#include <Common/assertion.h>
#include <Common/Container/StdUnorderedStringSet.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>

namespace ph
{

class SdlNamedOutputClauses final
{
public:
	/*! @brief Add a unique named output clauses.
	*/
	void addUnique(
		SdlOutputClauses clauses,
		std::string_view clausesName);

	/*! @brief Add a named output clauses. Potentially update the existing one.
	*/
	void addOrUpdate(
		SdlOutputClauses clauses,
		std::string_view clausesName);

	/*! @brief Add a named output clauses with generated name. Potentially update the existing one.
	@return Generated name. Never contain whitespaces.
	*/
	std::string_view addOrUpdate(SdlOutputClauses packet);

	void clear();

	std::size_t numNamedOutputClauses() const;

	/*! @brief Get named output clauses data.
	Ordering is the same as how they were added. The ones added later will have greater index.
	*/
	///@{
	const std::string& getName(std::size_t index) const;
	const SdlOutputClauses& getOutputClauses(std::size_t index) const;
	///@}

private:
	void add(
		SdlOutputClauses clauses,
		std::string_view clausesName);

	struct NamedClauses
	{
		std::string name;
		SdlOutputClauses clauses;
	};

	std::vector<NamedClauses> m_namedClauses;
	StdUnorderedStringSet m_clausesNames;
};

inline std::size_t SdlNamedOutputClauses::numNamedOutputClauses() const
{
	return m_namedClauses.size();
}

inline const std::string& SdlNamedOutputClauses::getName(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_namedClauses.size());

	return m_namedClauses[index].name;
}

inline const SdlOutputClauses& SdlNamedOutputClauses::getOutputClauses(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_namedClauses.size());

	return m_namedClauses[index].clauses;
}

inline void SdlNamedOutputClauses::clear()
{
	m_namedClauses.clear();
#if PH_DEBUG
	m_clausesNames.clear();
#endif
}

}// end namespace ph
