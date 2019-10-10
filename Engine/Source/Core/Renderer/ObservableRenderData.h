#pragma once

#include "Common/assertion.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ph
{

class ObservableRenderData final
{
public:
	void setLayerName(std::size_t index, const std::string& name);
	void setIntegerState(std::size_t index, const std::string& name);
	void setRealState(std::size_t index, const std::string& name);

	std::string getLayerName(std::size_t index) const;
	std::string getIntegerStateName(std::size_t index) const;
	std::string getRealStateName(std::size_t index) const;
	std::size_t numLayers() const;
	std::size_t numIntegerStates() const;
	std::size_t numRealStates() const;

private:
	std::vector<std::string> m_layerNames;
	std::vector<std::string> m_integerStateNames;
	std::vector<std::string> m_realStateNames;
};

// In-header Implementations:

inline void ObservableRenderData::setLayerName(const std::size_t index, const std::string& name)
{
	if(index >= m_layerNames.size())
	{
		m_layerNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_layerNames.size());
	m_layerNames[index] = name;
}

inline void ObservableRenderData::setIntegerState(const std::size_t index, const std::string& name)
{
	if(index >= m_integerStateNames.size())
	{
		m_integerStateNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_integerStateNames.size());
	m_integerStateNames[index] = name;
}

inline void ObservableRenderData::setRealState(const std::size_t index, const std::string& name)
{
	if(index >= m_realStateNames.size())
	{
		m_realStateNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_realStateNames.size());
	m_realStateNames[index] = name;
}

inline std::string ObservableRenderData::getLayerName(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_layerNames.size());
	return m_layerNames[index];
}

inline std::string ObservableRenderData::getIntegerStateName(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_integerStateNames.size());
	return m_integerStateNames[index];
}

inline std::string ObservableRenderData::getRealStateName(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_realStateNames.size());
	return m_realStateNames[index];
}

inline std::size_t ObservableRenderData::numLayers() const
{
	return m_layerNames.size();
}

inline std::size_t ObservableRenderData::numIntegerStates() const
{
	return m_integerStateNames.size();
}

inline std::size_t ObservableRenderData::numRealStates() const
{
	return m_realStateNames.size();
}

}// end namespace ph
