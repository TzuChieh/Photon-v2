#pragma once

#include <Common/primitive_type.h>

#include <cstddef>
#include <limits>

namespace ph
{

using CookPriority = uint8;
using CookLevel    = uint8;

enum class ECookPriority : CookPriority
{
	High = 1,
	Normal = std::numeric_limits<CookPriority>::max() / 2,
	Low
};

enum class ECookLevel : CookLevel
{
	First = 0,
	Normal = std::numeric_limits<CookLevel>::max() / 2,
	Last
};

/*! @brief Controls the order actors are cooked.
*/
class CookOrder final
{
public:
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
	CookOrder(ECookPriority::Normal, ECookLevel::Normal)
{}

inline CookOrder::CookOrder(const ECookPriority priority) : 
	CookOrder(priority, ECookLevel::Normal)
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
