#pragma once

#include <cstddef>

namespace ph::math
{

class IndexedKdtreeParams final
{
public:
	IndexedKdtreeParams();

	std::size_t getMaxNodeItems() const;
	int getTraversalCost() const;
	int getInteractCost() const;
	float getEmptyBonus() const;

private:
	std::size_t m_maxNodeItems;
	int         m_traversalCost;
	int         m_interactCost;
	float       m_emptyBonus;
};

// In-header Implementations:

inline IndexedKdtreeParams::IndexedKdtreeParams() : 
	m_maxNodeItems (1),
	m_traversalCost(1),
	m_interactCost (80),
	m_emptyBonus   (0.5f)
{}

inline std::size_t IndexedKdtreeParams::getMaxNodeItems() const
{
	return m_maxNodeItems;
}

inline int IndexedKdtreeParams::getTraversalCost() const
{
	return m_traversalCost;
}

inline int IndexedKdtreeParams::getInteractCost() const
{
	return m_interactCost;
}

inline float IndexedKdtreeParams::getEmptyBonus() const
{
	return m_emptyBonus;
}

}// end namespace ph::math
