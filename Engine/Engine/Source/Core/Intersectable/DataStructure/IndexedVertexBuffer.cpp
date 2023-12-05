#include "Core/Intersectable/DataStructure/IndexedVertexBuffer.h"
#include "Common/logging.h"
#include "Math/math.h"
#include "Math/Geometry/geometry.h"
#include "Math/TVector2.h"

#include <Common/exception.h>

#include <cstring>
#include <cmath>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IndexedVertexBuffer, Core);


IndexedVertexBuffer::Entry::Entry()
	: u_strideOffset(INVALID_STRIDE_VALUE)
	, strideSize(INVALID_STRIDE_VALUE)
	, element(EVertexElement::Float32)
	, numElements(0)
	, shouldNormalize(false)
{
	PH_ASSERT(isEmpty());
}

IndexedVertexBuffer::AttributeDeclaration::AttributeDeclaration()
	: strideOffset(Entry::INVALID_STRIDE_VALUE)
	, strideSize(Entry::INVALID_STRIDE_VALUE)
	, element(EVertexElement::Float32)
	, numElements(0)
	, shouldNormalize(false)
{
	PH_ASSERT(isEmpty());
}

IndexedVertexBuffer::IndexedVertexBuffer()
	: m_attributeTypeToEntryIndex()
	, m_entries()
	, m_numEntries(0)
	, m_byteBuffer(nullptr)
	, m_byteBufferSize(0)
	, m_vertexSize(0)
{
	PH_ASSERT(!isAllocated());

	m_attributeTypeToEntryIndex.fill(MAX_ENTRIES);
}

void IndexedVertexBuffer::declareAttribute(
	const EVertexAttribute attribute,
	const EVertexElement   element,
	const std::size_t      numElements,
	const bool             shouldNormalize)
{
	declareAttribute(
		attribute,
		element,
		numElements,
		Entry::INVALID_STRIDE_VALUE,
		Entry::INVALID_STRIDE_VALUE,
		shouldNormalize);
}

void IndexedVertexBuffer::declareAttribute(
	const EVertexAttribute attribute,
	const EVertexElement element,
	const std::size_t numElements,
	const std::size_t strideOffset,
	const std::size_t strideSize,
	const bool shouldNormalize)
{
	if(attribute >= EVertexAttribute::SIZE ||
	   element >= EVertexElement::SIZE ||
	   numElements == 0)
	{
		throw_formatted<InvalidArgumentException>(
			"invalid input parameter detected: attribute = {}, element = {}, numElements = {}",
			enum_to_value(attribute), enum_to_value(element), numElements);
	}

	if(m_attributeTypeToEntryIndex[enum_to_value(attribute)] != MAX_ENTRIES)
	{
		throw_formatted<InvalidArgumentException>(
			"redeclaring existing vertex attribute {}",
			enum_to_value(attribute));
	}

	auto entryIndex = m_numEntries;
	PH_ASSERT_LT(entryIndex, m_entries.size());

	// Start filling new entry information

	Entry inputEntry;
	inputEntry.element = element;

	if(numElements <= 3)
	{
		if(element == EVertexElement::OctahedralUnitVec3_32 || element == EVertexElement::OctahedralUnitVec3_24)
		{
			if(numElements != 0 && numElements != 3)
			{
				PH_LOG(IndexedVertexBuffer, 
					"Octahedral unit vector is defined to have 3 elements. The specified number ({}) is ignored.",
					numElements);
			}

			inputEntry.numElements = 3;
		}
		else
		{
			inputEntry.numElements = lossless_integer_cast<uint8>(numElements);
		}	
	}
	else
	{
		throw InvalidArgumentException("Cannot handle more than 3 elements in a single attribute.");
	}
	
	inputEntry.shouldNormalize = shouldNormalize ? true : false;
	inputEntry.u_strideOffset = strideOffset;
	inputEntry.strideSize = strideSize;

	// Writing new entry information
	// 
	// Note: Some info such as vertex size are not set here since user may still declare new
	// entries. They are set in `allocate()` instead when those info can be determined.

	m_entries[entryIndex] = inputEntry;
	m_attributeTypeToEntryIndex[enum_to_value(attribute)] = entryIndex;
	++m_numEntries;
}

void IndexedVertexBuffer::allocate(const std::size_t numVertices)
{
	ensureConsistentVertexLayout();

	// Calculate vertex size (sum of size of each attribute) and byte offset for each attribute

	std::array<std::size_t, MAX_ENTRIES> aosByteOffsetInVertex{};
	std::size_t currentVertexSize = 0;
	for(std::size_t entryIndex = 0; entryIndex < m_numEntries; ++entryIndex)
	{
		Entry& entry = m_entries[entryIndex];
		aosByteOffsetInVertex[entryIndex] = currentVertexSize;

		switch(entry.element)
		{
		case EVertexElement::Float32:
		case EVertexElement::Int32:
			currentVertexSize += 4 * entry.numElements;
			break;

		case EVertexElement::Float16:
		case EVertexElement::Int16:
			currentVertexSize += 2 * entry.numElements;
			break;

		case EVertexElement::OctahedralUnitVec3_32:
			currentVertexSize += 4;
			break;

		case EVertexElement::OctahedralUnitVec3_24:
			currentVertexSize += 3;
			break;

		default:
			throw InvalidArgumentException("Cannot allocate storage for invalid vertex element type.");
			break;
		}
	}

	m_vertexSize = lossless_integer_cast<decltype(m_vertexSize)>(currentVertexSize);

	// Allocate storage for the entries

	// Possibly clear existing buffer first to reduce memory usage
	m_byteBuffer = nullptr;

	// TODO: aligned byte buffer
	m_byteBufferSize = numVertices * m_vertexSize;
	m_byteBuffer = std::make_unique<std::byte[]>(m_byteBufferSize);

	// Set buffer offset information for each entry

	for(std::size_t entryIndex = 0; entryIndex < m_numEntries; ++entryIndex)
	{
		Entry& entry = m_entries[entryIndex];

		// Use AoS if no existing stride size is provided
		if(!entry.hasStrideInfo())
		{
			entry.u_attributeBuffer = m_byteBuffer.get() + aosByteOffsetInVertex[entryIndex];
			entry.strideSize = m_vertexSize;
		}
		// Use custom vertex layout
		else
		{
			entry.u_attributeBuffer = m_byteBuffer.get() + entry.u_strideOffset;
		}
	}

	if(m_byteBufferSize == 0)
	{
		PH_LOG_WARNING(IndexedVertexBuffer, "Allocated buffer with 0 size.");
	}
}

math::Vector3R IndexedVertexBuffer::getAttribute(const EVertexAttribute attribute, const std::size_t index) const
{
	PH_ASSERT(isAllocated());

	if(!hasEntry(attribute))
	{
		return math::Vector3R(0);
	}

	const Entry& entry = getEntry(attribute);
	PH_ASSERT(!entry.isEmpty());

	const std::byte* const bufferPtr = entry.u_attributeBuffer + index * entry.strideSize;
	PH_ASSERT(bufferPtr);

	math::Vector3R value(0);
	switch(entry.element)
	{
	case EVertexElement::Float32:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			float32 element;
			std::memcpy(&element, bufferPtr + ei * 4, 4);
			value[ei] = element;
		}
		break;

	case EVertexElement::Float16:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			uint16 fp16Bits;
			std::memcpy(&fp16Bits, bufferPtr + ei * 2, 2);
			value[ei] = math::fp16_bits_to_fp32(fp16Bits);
		}
		break;

	case EVertexElement::Int32:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			int32 element;
			std::memcpy(&element, bufferPtr + ei * 4, 4);

			value[ei] = entry.shouldNormalize
				? math::normalize_integer<real>(element)
				: static_cast<real>(element);
		}
		break;

	case EVertexElement::Int16:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			int16 element;
			std::memcpy(&element, bufferPtr + ei * 2, 2);

			value[ei] = entry.shouldNormalize
				? math::normalize_integer<real>(element)
				: static_cast<real>(element);
		}
		break;

	case EVertexElement::OctahedralUnitVec3_32:
		{
			math::TVector2<uint16> encodedBits;
			std::memcpy(&encodedBits.x(), bufferPtr + 0 * 2, 2);
			std::memcpy(&encodedBits.y(), bufferPtr + 1 * 2, 2);

			const math::Vector2R encodedVal(
				math::normalize_integer<real>(encodedBits.x()),
				math::normalize_integer<real>(encodedBits.y()));

			value = math::octahedron_unit_vector_decode(encodedVal);
		}
		break;

	case EVertexElement::OctahedralUnitVec3_24:
		{
			// Read 3 bytes (we use only the first 3 bytes of the uint32)
			uint32 packedBits;
			std::memcpy(&packedBits, bufferPtr, 3);

			const math::TVector2<uint32> encodedBits(
				(packedBits & 0x00000FFF),
				(packedBits & 0x00FFF000) >> 12);

			PH_ASSERT_LE(encodedBits.x(), 4096 - 1);
			PH_ASSERT_LE(encodedBits.y(), 4096 - 1);

			const math::Vector2R encodedVal(
				static_cast<real>(encodedBits.x()) / 4095.0_r,
				static_cast<real>(encodedBits.y()) / 4095.0_r);

			value = math::octahedron_unit_vector_decode(encodedVal);
		}
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}

	return value;
}

void IndexedVertexBuffer::setAttribute(
	const EVertexAttribute attribute,
	const std::size_t      index,
	const math::Vector3R&  value)
{
	PH_ASSERT(isAllocated());

	if(!hasEntry(attribute))
	{
		throw_formatted<InvalidArgumentException>(
			"Setting value to an empty vertex atrribute {}.", 
			enum_to_value(attribute));
	}

	const Entry& entry = getEntry(attribute);
	PH_ASSERT(!entry.isEmpty());

	std::byte* const bufferPtr = entry.u_attributeBuffer + index * entry.strideSize;
	PH_ASSERT(bufferPtr);

	switch(entry.element)
	{
	case EVertexElement::Float32:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			const auto element = static_cast<float32>(value[ei]);
			std::memcpy(bufferPtr + ei * 4, &element, 4);
		}
		break;

	case EVertexElement::Float16:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			const uint16 fp16Bits = math::fp32_to_fp16_bits(static_cast<float32>(value[ei]));
			std::memcpy(bufferPtr + ei * 2, &fp16Bits, 2);
		}
		break;

	case EVertexElement::Int32:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			if(entry.shouldNormalize && std::abs(value[ei]) > 1.0_r)
			{
				throw InvalidArgumentException("Cannot set un-normalized value to a normalized entry.");
			}

			const auto element = entry.shouldNormalize
				? math::quantize_normalized_float<int32>(value[ei])
				: static_cast<int32>(std::round(value[ei]));
			std::memcpy(bufferPtr + ei * 4, &element, 4);
		}
		break;

	case EVertexElement::Int16:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			if(entry.shouldNormalize && std::abs(value[ei]) > 1.0_r)
			{
				throw InvalidArgumentException("Cannot set un-normalized value to a normalized entry.");
			}

			const auto element = entry.shouldNormalize
				? math::quantize_normalized_float<int16>(value[ei])
				: static_cast<int16>(std::round(value[ei]));
			std::memcpy(bufferPtr + ei * 2, &element, 2);
		}
		break;

	case EVertexElement::OctahedralUnitVec3_32:
		{
			const math::Vector2R encodedVal = math::octahedron_unit_vector_encode(value);

			const math::TVector2<uint16> encodedBits(
				math::quantize_normalized_float<uint16>(encodedVal.x()),
				math::quantize_normalized_float<uint16>(encodedVal.y()));

			std::memcpy(bufferPtr + 0 * 2, &encodedBits.x(), 2);
			std::memcpy(bufferPtr + 1 * 2, &encodedBits.y(), 2);
		}
		break;

	case EVertexElement::OctahedralUnitVec3_24:
		{
			const math::Vector2R encodedVal = math::octahedron_unit_vector_encode(value);

			const math::TVector2<uint32> encodedBits(
				static_cast<uint32>(std::round(encodedVal.x() * 4095.0_r)),
				static_cast<uint32>(std::round(encodedVal.y() * 4095.0_r)));

			PH_ASSERT_LE(encodedBits.x(), 4096 - 1);
			PH_ASSERT_LE(encodedBits.y(), 4096 - 1);

			// Write 3 bytes (we use only the first 3 bytes of the uint32)
			const uint32 packedBits = (encodedBits.x() & 0x00000FFF) | ((encodedBits.y() & 0x00000FFF) << 12);
			std::memcpy(bufferPtr, &packedBits, 3);
		}
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}
}

void IndexedVertexBuffer::setVertices(const std::byte* const srcBytes, const std::size_t numBytes, const std::size_t dstOffset)
{
	PH_ASSERT(srcBytes);
	PH_ASSERT(isAllocated());

	if(dstOffset + numBytes > m_byteBufferSize)
	{
		throw_formatted<InvalidArgumentException>(
			"Copying {} bytes will overflow the vertex buffer (buffer-size: {} bytes, buffer-offset: {}).",
			numBytes, numBytes, dstOffset);
	}

	std::memcpy(&(m_byteBuffer[dstOffset]), srcBytes, numBytes);
}

auto IndexedVertexBuffer::getAttributeDeclaration(const EVertexAttribute attribute) const
-> AttributeDeclaration
{
	// Can only be called after allocation
	PH_ASSERT(isAllocated());

	if(!hasEntry(attribute))
	{
		return AttributeDeclaration();
	}

	const Entry& entry = getEntry(attribute);
	
	AttributeDeclaration declaration;
	declaration.strideOffset = lossless_integer_cast<std::size_t>(entry.u_attributeBuffer - m_byteBuffer.get());
	declaration.strideSize = entry.strideSize;
	declaration.element = entry.element;
	declaration.numElements = entry.numElements;
	declaration.shouldNormalize = entry.shouldNormalize;

	return declaration;
}

void IndexedVertexBuffer::ensureConsistentVertexLayout() const
{
	if(m_numEntries == 0)
	{
		return;
	}

	bool shouldHaveStrideInfo = m_entries[0].hasStrideInfo();
	for(std::size_t entryIndex = 1; entryIndex < m_numEntries; ++entryIndex)
	{
		if(m_entries[entryIndex].hasStrideInfo() != shouldHaveStrideInfo)
		{
			throw InvalidArgumentException(
				"Inconsistent stride info detected. Attributes must all use automatic stride "
				"size/offset (AoS) or all with custom stride size/offset.");
		}
	}
}

}// end namespace ph
