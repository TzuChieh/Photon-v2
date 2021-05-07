#pragma once

#include "Common/assertion.h"

#include <string>
#include <vector>

namespace ph
{

class ValueClauses final
{
public:
	struct ValueClause
	{
		std::string type;
		std::string name;
		std::string value;

		std::string genPrettyName() const;
	};

	void add(std::string type, std::string name, std::string value);
	void remove(std::size_t index);
	void removeBySwapPop(std::size_t index);

	std::size_t size() const;
	std::string toString() const;

	const ValueClause& operator [] (std::size_t index) const;

private:
	std::vector<ValueClause> m_clauses;
};

// In-header Implementations:

inline void ValueClauses::add(std::string type, std::string name, std::string value)
{
	m_clauses.push_back({std::move(type), std::move(name), std::move(value)});
}

inline void ValueClauses::remove(const std::size_t index)
{
	PH_ASSERT_LT(index, m_clauses.size());

	m_clauses.erase(m_clauses.begin() + index);
}

inline void ValueClauses::removeBySwapPop(const std::size_t index)
{
	PH_ASSERT(!m_clauses.empty());
	PH_ASSERT_LT(index, m_clauses.size());

	using std::swap;

	swap(m_clauses[index], m_clauses.back());
	m_clauses.pop_back();
}

inline std::string ValueClauses::ValueClause::genPrettyName() const
{
	return "type: " + type + ", name: " + name;
}

inline std::size_t ValueClauses::size() const
{
	return m_clauses.size();
}

inline std::string ValueClauses::toString() const
{
	return std::to_string(m_clauses.size()) + " value clauses";
}

inline const ValueClause& operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, m_clauses.size());

	return m_clauses[index];
}

}// end namespace ph
