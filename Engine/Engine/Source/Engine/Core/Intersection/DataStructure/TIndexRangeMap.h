#pragma once

#include <Common/primitive_type.h>
#include <Common/assertion.h>

#include <memory>
#include <algorithm>
#include <utility>

namespace ph
{

/*! @brief Maps contiguous index ranges to values.
Each entry stores the inclusive max index of one range. The number of entries is
the number of contiguous ranges, not the number of source indices or unique values.
*/
template<typename Index, typename Value>
class TIndexRangeMap final
{
public:
	TIndexRangeMap()
		: m_runLengthToValue(nullptr)
		, m_numEntries(0)
	{}

	explicit TIndexRangeMap(uint32 numEntries)
		: m_runLengthToValue(std::make_unique<ValueRunLength[]>(numEntries))
		, m_numEntries(numEntries)
	{}

	/*! @brief Gets the value for a given index.
	Extraction is @f$ O(logN) @f$, where @f$ N @f$ is the number of index ranges.
	*/
	const Value& get(const Index index) const
	{
		PH_ASSERT(m_runLengthToValue);

		ValueRunLength queryInput;
		queryInput.maxIndex = index;

		const auto* entryPtr = std::lower_bound(
			m_runLengthToValue.get(), 
			m_runLengthToValue.get() + m_numEntries, 
			queryInput);

		PH_ASSERT(entryPtr < m_runLengthToValue.get() + m_numEntries);
		return entryPtr->value;
	}

	/*! @brief Sets a range entry by its inclusive max index.
	Entries must be set in ascending max-index order.
	*/
	void setRangeMap(uint32 entryIndex, Index maxIndex, Value value)
	{
		PH_ASSERT_LT(entryIndex, m_numEntries);
		PH_ASSERT(entryIndex == 0 || m_runLengthToValue[entryIndex - 1].maxIndex < maxIndex);

		m_runLengthToValue[entryIndex] = ValueRunLength{maxIndex, value};
	}

	bool isEmpty() const
	{
		return m_numEntries == 0;
	}

	template<typename PerEntryOperation>
	void forEachEntry(PerEntryOperation op) const
	{
		for(uint32 entryIndex = 0; entryIndex < m_numEntries; ++entryIndex)
		{
			op(m_runLengthToValue[entryIndex].maxIndex, m_runLengthToValue[entryIndex].value);
		}
	}

	uint32 size() const
	{
		return m_numEntries;
	}

private:
	// Run-length encoding, tracking max index for a value
	struct ValueRunLength
	{
		Index maxIndex;
		Value value;

		bool operator == (const ValueRunLength& other) const
		{
			return (*this <=> other) == 0;
		}

		auto operator <=> (const ValueRunLength& other) const
		{
			// We only need max index to determine which run length an index belongs
			return maxIndex <=> other.maxIndex;
		}
	};

	std::unique_ptr<ValueRunLength[]> m_runLengthToValue;
	uint32 m_numEntries;
};

}// end namespace ph
