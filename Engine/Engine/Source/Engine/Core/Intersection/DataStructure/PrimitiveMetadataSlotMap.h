#pragma once

#include "Engine/Utility/TSortedVector.h"

#include <Common/primitive_type.h>
#include <Common/assertion.h>

namespace ph
{

/*!
Run-length encoding of face IDs to primitive metadata slot indices.
*/
class PrimitiveMetadataSlotMap final
{
public:
	/*! @brief Gets the slot index for the given face ID.
	Extraction is @f$ O(logN) @f$, where @f$ N @f$ is the number of primitive metadata clusters.
	*/
	uint32 getSlot(const uint64 faceId) const;

	/*! @brief Adds a mapping for a new slot by its max face ID.
	If the mapping already exists, it will not be added again.
	*/
	void addSlot(uint32 slot, uint64 maxFaceId);

	void reallocateToFit();

private:
	struct SlotRunLength
	{
		uint64 faceId;
		uint32 slot;

		bool operator == (const SlotRunLength& other) const
		{
			return (*this <=> other) == 0;
		}

		auto operator <=> (const SlotRunLength& other) const
		{
			// We only need max face ID to determine which run length a face ID belongs
			return faceId <=> other.faceId;
		}
	};

	TSortedVector<SlotRunLength> m_maxFaceIdToSlot;
};

inline uint32 PrimitiveMetadataSlotMap::getSlot(const uint64 faceId) const
{
	SlotRunLength queryInput;
	queryInput.faceId = faceId;

	const uint32 runLengthIndex = m_maxFaceIdToSlot.lowerBound(queryInput);
	return m_maxFaceIdToSlot.get(runLengthIndex).slot;
}

inline void PrimitiveMetadataSlotMap::addSlot(const uint32 slot, const uint64 maxFaceId)
{
	m_maxFaceIdToSlot.addUniqueValue(SlotRunLength{maxFaceId, slot});
}

inline void PrimitiveMetadataSlotMap::reallocateToFit()
{
	m_maxFaceIdToSlot.shrinkToFit();
}

}// end namespace ph
