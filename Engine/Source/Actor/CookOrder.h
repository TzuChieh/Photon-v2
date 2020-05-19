#pragma once

#include <cstddef>
#include <limits>

namespace ph
{

using CookPriority = int;
using CookLevel    = int;

enum class ECookPriority : CookPriority
{
	HIGH = 1,
	NORMAL = std::numeric_limits<CookPriority>::max() / 2,
	LOW
};

enum class ECookLevel : CookLevel
{
	FIRST = 0,
	NORMAL = std::numeric_limits<CookPriority>::max() / 2,
	LAST
};

/*! @brief Controls the order actors are cooked.
*/
struct CookOrder
{
	CookPriority priority;
	CookLevel    level;

	/*! @brief Cooks normally.
	*/
	CookOrder();

	explicit CookOrder(ECookPriority priority);
	CookOrder(ECookPriority priority, ECookLevel level);
	CookOrder(CookPriority priority, CookLevel level);

	bool operator < (const CookOrder& rhs) const;
};

// In-header Implementations:

inline CookOrder::CookOrder() : 
	CookOrder(ECookPriority::NORMAL, ECookLevel::NORMAL)
{}

inline CookOrder::CookOrder(const ECookPriority priority) : 
	CookOrder(priority, ECookLevel::NORMAL)
{}

inline CookOrder::CookOrder(const ECookPriority priority, const ECookLevel level) : 
	CookOrder(static_cast<CookPriority>(priority), static_cast<CookLevel>(level))
{}

inline CookOrder::CookOrder(const CookPriority priority, const CookLevel level) : 
	priority(priority), level(level)
{}

inline bool CookOrder::operator < (const CookOrder& rhs) const
{
	// Smaller level takes precedence over priority
	return this->level != rhs.level ? 
		this->level < rhs.level : 
		this->priority < rhs.priority;
}

}// end namespace ph
