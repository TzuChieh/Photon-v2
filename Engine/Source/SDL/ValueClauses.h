#pragma once

#include "Common/assertion.h"
#include "SDL/SdlInputPayload.h"

#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace ph
{

class ValueClauses final
{
public:
	struct Clause
	{
		std::string     type;
		std::string     name;
		SdlInputPayload payload;

		inline Clause() = default;
		Clause(std::string type, std::string name, std::string value);
		Clause(std::string type, std::string name, std::string value, std::string tag);

		std::string genPrettyName() const;
		std::string toString() const;
	};

	/*! @brief Add a new clause.
	*/
	void add(Clause clause);

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
	const Clause& operator [] (std::size_t index) const;

private:
	std::vector<Clause> m_clauses;
};

// In-header Implementations:

inline void ValueClauses::add(Clause clause)
{
	m_clauses.push_back(std::move(clause));
}

inline void ValueClauses::add(std::string type, std::string name, std::string value)
{
	add(Clause(std::move(type), std::move(name), std::move(value)));
}

inline void ValueClauses::add(std::string type, std::string name, std::string value, std::string tag)
{
	add(Clause(std::move(type), std::move(name), std::move(value), std::move(tag)));
}

inline void ValueClauses::consume(const std::size_t index)
{
	PH_ASSERT_LT(index, m_clauses.size());

	m_clauses.erase(m_clauses.begin() + index);
}

inline void ValueClauses::consumeBySwapPop(const std::size_t index)
{
	PH_ASSERT(!m_clauses.empty());
	PH_ASSERT_LT(index, m_clauses.size());

	using std::swap;

	swap(m_clauses[index], m_clauses.back());
	m_clauses.pop_back();
}

inline void ValueClauses::clear()
{
	m_clauses.clear();
}

inline std::size_t ValueClauses::size() const
{
	return m_clauses.size();
}

inline std::string ValueClauses::toString() const
{
	return std::to_string(m_clauses.size()) + " value clauses";
}

inline const ValueClauses::Clause& ValueClauses::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_clauses.size());

	return m_clauses[index];
}

inline ValueClauses::Clause::Clause(std::string type, std::string name, std::string value) :
	type   (std::move(type)),
	name   (std::move(name)),
	payload(std::move(value))
{}

inline ValueClauses::Clause::Clause(std::string type, std::string name, std::string value, std::string tag) :
	type   (std::move(type)),
	name   (std::move(name)),
	payload(std::move(value), std::move(tag))
{}

inline std::string ValueClauses::Clause::genPrettyName() const
{
	return "type: " + type + ", name: " + name;
}

inline std::string ValueClauses::Clause::toString() const
{
	return "type: " + type + ", name: " + name + ", payload = [" + payload.toString() + "]";
}

}// end namespace ph
