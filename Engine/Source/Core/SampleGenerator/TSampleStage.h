#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class SampleGenerator;

template<typename T>
class TSampleStage final
{
public:
	friend class SampleGenerator;

	TSampleStage(const TSampleStage& other) :
		m_stageIndex(other.m_stageIndex)
	{}

	inline TSampleStage& operator = (const TSampleStage& rhs)
	{
		m_stageIndex = rhs.m_stageIndex;

		return *this;
	}

private:
	std::size_t m_stageIndex;

	inline explicit TSampleStage(const std::size_t stageIndex) :
		m_stageIndex(stageIndex)
	{}
};

}// end namespace ph