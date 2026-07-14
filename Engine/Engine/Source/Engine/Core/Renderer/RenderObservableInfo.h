#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <string>
#include <vector>

namespace ph
{

/*! @brief Describes data that clients can observe during or after rendering.
- Layer indices correspond to frame retrieval indices.
- Layer, integer-stat, and real-stat index spaces are independent.
*/
class RenderObservableInfo final
{
public:
	void setProgressTimeMeasurement(const std::string& measurement);

	/*! @brief Set the display name for a frame output layer.
	*/
	void setLayer(uint32 index, const std::string& name);

	/*! @brief Set display name for `RenderStats::getInteger(index)`.
	This index space is independent from real statistic indices.
	*/
	void setIntegerStat(uint32 index, const std::string& name);

	/*! @brief Set display name for `RenderStats::getReal(index)`.
	This index space is independent from integer statistic indices.
	*/
	void setRealStat(uint32 index, const std::string& name);

	std::string getProgressTimeMeasurement() const;
	std::string getLayerName(uint32 index) const;
	std::string getIntegerStatName(uint32 index) const;
	std::string getRealStatName(uint32 index) const;
	uint32 numLayers() const;
	uint32 numIntegerStats() const;
	uint32 numRealStats() const;

private:
	std::string m_progressTimeMeasurement;
	std::vector<std::string> m_layerNames;
	std::vector<std::string> m_integerStatNames;
	std::vector<std::string> m_realStatNames;
};

// In-header Implementations:

inline void RenderObservableInfo::setProgressTimeMeasurement(const std::string& measurement)
{
	m_progressTimeMeasurement = measurement;
}

inline void RenderObservableInfo::setLayer(const uint32 index, const std::string& name)
{
	if(index >= m_layerNames.size())
	{
		m_layerNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_layerNames.size());
	m_layerNames[index] = name;
}

inline void RenderObservableInfo::setIntegerStat(const uint32 index, const std::string& name)
{
	if(index >= m_integerStatNames.size())
	{
		m_integerStatNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_integerStatNames.size());
	m_integerStatNames[index] = name;
}

inline void RenderObservableInfo::setRealStat(const uint32 index, const std::string& name)
{
	if(index >= m_realStatNames.size())
	{
		m_realStatNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_realStatNames.size());
	m_realStatNames[index] = name;
}

inline std::string RenderObservableInfo::getProgressTimeMeasurement() const
{
	return m_progressTimeMeasurement;
}

inline std::string RenderObservableInfo::getLayerName(const uint32 index) const
{
	PH_ASSERT_LT(index, m_layerNames.size());
	return m_layerNames[index];
}

inline std::string RenderObservableInfo::getIntegerStatName(const uint32 index) const
{
	PH_ASSERT_LT(index, m_integerStatNames.size());
	return m_integerStatNames[index];
}

inline std::string RenderObservableInfo::getRealStatName(const uint32 index) const
{
	PH_ASSERT_LT(index, m_realStatNames.size());
	return m_realStatNames[index];
}

inline uint32 RenderObservableInfo::numLayers() const
{
	return static_cast<uint32>(m_layerNames.size());
}

inline uint32 RenderObservableInfo::numIntegerStats() const
{
	return static_cast<uint32>(m_integerStatNames.size());
}

inline uint32 RenderObservableInfo::numRealStats() const
{
	return static_cast<uint32>(m_realStatNames.size());
}

}// end namespace ph
