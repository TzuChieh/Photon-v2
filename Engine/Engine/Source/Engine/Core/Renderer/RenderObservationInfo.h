#pragma once

#include <Common/assertion.h>

#include <cstddef>
#include <string>
#include <vector>

namespace ph
{

/*! @brief Meta information for data related to ongoing render operation.
Contains three independent groups of metadata:
1. Output layers (`setLayer()`), usually corresponding to film/layer outputs.
2. Integer stats (`setIntegerStat()`), names for `RenderStats::getInteger(index)`.
3. Real stats (`setRealStat()`), names for `RenderStats::getReal(index)`.
Layer definitions and stat definitions are independent; stats are not required to be per-layer.
*/
class RenderObservationInfo final
{
public:
	void setProgressTimeMeasurement(const std::string& measurement);

	/*! @brief Set name of an output layer.
	`index` is the layer index used by frame retrieval APIs.
	*/
	void setLayer(std::size_t index, const std::string& name);

	/*! @brief Set display name for `RenderStats::getInteger(index)`.
	This index space is independent from layer index space.
	*/
	void setIntegerStat(std::size_t index, const std::string& name);

	/*! @brief Set display name for `RenderStats::getReal(index)`.
	This index space is independent from layer index space.
	*/
	void setRealStat(std::size_t index, const std::string& name);

	std::string getProgressTimeMeasurement() const;
	std::string getLayerName(std::size_t index) const;
	std::string getIntegerStatName(std::size_t index) const;
	std::string getRealStatName(std::size_t index) const;
	std::size_t numLayers() const;
	std::size_t numIntegerStats() const;
	std::size_t numRealStats() const;

private:
	std::string m_progressTimeMeasurement;
	std::vector<std::string> m_layerNames;
	std::vector<std::string> m_integerStatNames;
	std::vector<std::string> m_realStatNames;
};

// In-header Implementations:

inline void RenderObservationInfo::setProgressTimeMeasurement(const std::string& measurement)
{
	m_progressTimeMeasurement = measurement;
}

inline void RenderObservationInfo::setLayer(const std::size_t index, const std::string& name)
{
	if(index >= m_layerNames.size())
	{
		m_layerNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_layerNames.size());
	m_layerNames[index] = name;
}

inline void RenderObservationInfo::setIntegerStat(const std::size_t index, const std::string& name)
{
	if(index >= m_integerStatNames.size())
	{
		m_integerStatNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_integerStatNames.size());
	m_integerStatNames[index] = name;
}

inline void RenderObservationInfo::setRealStat(const std::size_t index, const std::string& name)
{
	if(index >= m_realStatNames.size())
	{
		m_realStatNames.resize(index + 1);
	}

	PH_ASSERT_LT(index, m_realStatNames.size());
	m_realStatNames[index] = name;
}

inline std::string RenderObservationInfo::getProgressTimeMeasurement() const
{
	return m_progressTimeMeasurement;
}

inline std::string RenderObservationInfo::getLayerName(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_layerNames.size());
	return m_layerNames[index];
}

inline std::string RenderObservationInfo::getIntegerStatName(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_integerStatNames.size());
	return m_integerStatNames[index];
}

inline std::string RenderObservationInfo::getRealStatName(const std::size_t index) const
{
	PH_ASSERT_LT(index, m_realStatNames.size());
	return m_realStatNames[index];
}

inline std::size_t RenderObservationInfo::numLayers() const
{
	return m_layerNames.size();
}

inline std::size_t RenderObservationInfo::numIntegerStats() const
{
	return m_integerStatNames.size();
}

inline std::size_t RenderObservationInfo::numRealStats() const
{
	return m_realStatNames.size();
}

}// end namespace ph
