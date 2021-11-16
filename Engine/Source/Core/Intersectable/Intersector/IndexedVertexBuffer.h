#pragma once

#include "Common/primitive_type.h"
#include "Utility/TArrayAsVector.h"
#include "Math/TVector3.h"

#include <memory>
#include <cstddef>
#include <limits>

namespace ph
{

enum class EVertexContent
{
	Position_0 = 0,
	Normal_0,
	Tangent_0,
	TexCoord_0,
	TexCoord_1,
	Color_0,

	// Special values
	NUM
};

enum class EVertexElement
{
	VE_Float16 = 0,
	VE_Float32,
	VE_Int16,
	VE_Int32,
	VE_OctahedralUnitVec24,
	VE_OctahedralUnitVec32,

	// Special values
	NUM
};

class IndexedVertexBuffer final
{
public:
	IndexedVertexBuffer();

	void addEntry(
		EVertexContent content,
		EVertexElement element,
		std::size_t    numElements = 0,
		bool           isNormalized = false);

	void allocate();

private:
	struct Entry final
	{
		EVertexContent content;
		EVertexElement element;

		/*! @brief Number of elements in this entry. Expected to be within [1, 3]. */
		uint8 numElements : 2;

		/*! @brief Whether the stored value is in [0, 1] ([-1, 1] for signed types). 
		This attribute is for integral types only. Take uint8 for example, if this attribute is true, an
		input value of 255 will be converted to 1.0 on load; otherwise, the value is converted to float
		as-is (i.e., 255 becomes 255.0).
		*/
		uint8 isNormalized : 1;

		Entry();
	};

	TArrayAsVector<Entry, static_cast<std::size_t>(EVertexContent::NUM)> m_entries;

	std::unique_ptr<std::byte[]> m_byteBuffer;
	std::size_t                  m_byteBufferSize;
};

}// end namespace ph
