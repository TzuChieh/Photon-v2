#pragma once

#include "SDL/SdlInputClause.h"

#include <Common/assertion.h>

#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace ph
{

class SdlInputClauses final
{
public:
	/*! @brief Add a new clause.
	*/
	void add(SdlInputClause clause);

	/*! @brief Add a new clause.
	*/
	void add(std::string type, std::string name, std::string value);

	/*! @brief Add a new clause.
	*/
	void add(std::string type, std::string name, std::string value, std::string tag);

	/*! @brief Remove a clause by index. Preserves the order of remaining clauses.
	*/
	void consume(std::size_t index);

	/*! @brief Remove a clause by index. Does not preserve the order of remaining clauses.
	*/
	void consumeBySwapPop(std::size_t index);

	/*! @brief Clear all stored data.
	*/
	void clear();

	/*! @brief Get number of clauses.
	*/
	std::size_t size() const;

	std::string toString() const;

	/*! @brief Get clause by index.
	*/
	const SdlInputClause& operator [] (std::size_t index) const;

private:
	std::vector<SdlInputClause> m_clauses;
};

// In-header Implementations:

inline void SdlInputClauses::add(SdlInputClause clause)
{
	m_clauses.push_back(std::move(clause));
}

inline void SdlInputClauses::add(std::string type, std::string name, std::string value)
{
	add(SdlInputClause(std::move(type), std::move(name), std::move(value)));
}

inline void SdlInputClauses::add(std::string type, std::string name, std::string value, std::string tag)
{
	add(SdlInputClause(std::move(type), std::move(name), std::move(value), std::move(tag)));
}

inline void SdlInputClauses::consume(const std::size_t index)
{
	PH_ASSERT_LT(index, m_clauses.size());

	m_clauses.erase(m_clauses.begin() + index);
}

inline void SdlInputClauses::consumeBySwapPop(const std::size_t index)
{
	PH_ASSERT(!m_clauses.empty());
	PH_ASSERT_LT(index, m_clauses.size());

	using std::swap;

	swap(m_clauses[index], m_clauses.back());
	m_clauses.pop_back();
}

inline void SdlInputClauses::clear()
{
	m_clauses.clear();
}

inline std::size_t SdlInputClauses::size() const
{
	return m_clauses.size();
}

inline std::string SdlInputClauses::toString() const
{
	return std::to_string(m_clauses.size()) + " value clauses";
}

inline const SdlInputClause& SdlInputClauses::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_clauses.size());
	return m_clauses[index];
}

}// end namespace ph
