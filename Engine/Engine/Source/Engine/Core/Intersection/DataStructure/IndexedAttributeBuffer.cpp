#include "Engine/Core/Intersection/DataStructure/IndexedAttributeBuffer.h"
#include "Engine/Math/Geometry/geometry.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/math.h"

#include <Common/exceptions.h>
#include <Common/logging.h>
#include <Common/os.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <memory>
#include <new>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IndexedAttributeBuffer, Core);


IndexedAttributeBuffer::Entry::Entry()
	: u_strideOffset(INVALID_STRIDE_VALUE)
	, strideSize(INVALID_STRIDE_VALUE)
	, element(EAttributeElement::Float32)
	, domain(EAttributeDomain::Vertex)
	, numElements(0)
	, shouldNormalize(false)
{}

IndexedAttributeBuffer::AttributeDeclaration::AttributeDeclaration()
	: strideOffset(Entry::INVALID_STRIDE_VALUE)
	, strideSize(Entry::INVALID_STRIDE_VALUE)
	, element(EAttributeElement::Float32)
	, domain(EAttributeDomain::Vertex)
	, numElements(0)
	, shouldNormalize(false)
{}

IndexedAttributeBuffer::IndexedAttributeBuffer()
	: m_byteBuffer(makeByteBuffer(DECLARATION_STORAGE_SIZE, alignof(Entry)))
	, m_attributeMask(0)
#if PH_DEBUG
	, m_isAttributeAllocated(false)
#endif
{
	Entry* const entries = start_implicit_lifetime_as_array<Entry>(m_byteBuffer.get(), MAX_ENTRIES);
	for(std::size_t entryIndex = 0; entryIndex < MAX_ENTRIES; ++entryIndex)
	{
		std::construct_at(entries + entryIndex);
	}
}

void IndexedAttributeBuffer::declareAttribute(
	const EPrimitiveAttribute attribute,
	const EAttributeDomain domain,
	const EAttributeElement element,
	const std::size_t numElements,
	const bool shouldNormalize)
{
	declareAttribute(
		attribute,
		domain,
		element,
		numElements,
		Entry::INVALID_STRIDE_VALUE,
		Entry::INVALID_STRIDE_VALUE,
		shouldNormalize);
}

void IndexedAttributeBuffer::declareAttribute(
	const EPrimitiveAttribute attribute,
	const EAttributeDomain domain,
	const EAttributeElement element,
	const std::size_t numElements,
	const std::size_t strideOffset,
	const std::size_t strideSize,
	const bool shouldNormalize)
{
	PH_ASSERT(!m_isAttributeAllocated);

	if(attribute >= EPrimitiveAttribute::SIZE ||
	   domain >= EAttributeDomain::SIZE ||
	   element >= EAttributeElement::SIZE ||
	   numElements == 0)
	{
		throw_formatted<InvalidArgumentException>(
			"invalid input parameter detected: attribute = {}, domain = {}, element = {}, numElements = {}",
			enum_to_value(attribute), enum_to_value(domain), enum_to_value(element), numElements);
	}

	if(hasEntry(attribute))
	{
		throw_formatted<InvalidArgumentException>(
			"redeclaring existing primitive attribute {}",
			enum_to_value(attribute));
	}

	// Start filling new entry information

	Entry inputEntry;
	inputEntry.element = element;
	inputEntry.domain = domain;

	if(numElements <= 3)
	{
		if(element == EAttributeElement::OctahedralUnitVec3_32 ||
		   element == EAttributeElement::OctahedralUnitVec3_24 ||
		   element == EAttributeElement::OctahedralUnitVec3_31_CustomBits_1)
		{
			if(numElements != 3)
			{
				PH_LOG(IndexedAttributeBuffer, Note,
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

	inputEntry.shouldNormalize = shouldNormalize;
	inputEntry.u_strideOffset = strideOffset;
	inputEntry.strideSize = strideSize;

	// Writing new entry information
	//
	// Note: Some info such as automatic strides are not set here since users may still declare new
	// entries. They are set in `allocate()` when the complete layout is known.

	getEntries()[enum_to_value(attribute)] = inputEntry;
	m_attributeMask |= math::flag_bit<AttributeMask>(enum_to_value(attribute));
}

auto IndexedAttributeBuffer::allocate(const std::size_t numVertices, const std::size_t numFaces)
-> IndexedAttributeBufferWriter
{
	PH_ASSERT(!m_isAttributeAllocated);
	ensureConsistentAttributeLayout();

	const AttributeMask numEntries = this->numEntries();

	// Gather and compact declarations
	std::array<Entry, MAX_ENTRIES> compactEntries;
	AttributeMask numCompactedEntries = 0;
	for(AttributeMask attributeIndex = 0; attributeIndex < MAX_ENTRIES; ++attributeIndex)
	{
		const auto attribute = static_cast<EPrimitiveAttribute>(attributeIndex);
		if(hasEntry(attribute))
		{
			compactEntries[numCompactedEntries++] = getDeclaredEntry(attribute);
		}
	}

	// Resolve offsets and storage sizes depending on custom/auto layout
	const bool useCustomLayout = numEntries > 0 && compactEntries[0].hasStrideInfo();
	std::size_t attributeStorageSize = 0;
	if(useCustomLayout)
	{
		for(AttributeMask entryIndex = 0; entryIndex < numEntries; ++entryIndex)
		{
			const Entry& entry = compactEntries[entryIndex];

			// Find the max space this attribute may take
			const std::size_t numAttributeValues = entry.domain == EAttributeDomain::Vertex
				? numVertices : numFaces;
			std::size_t entryEnd = entry.u_strideOffset;
			if(numAttributeValues > 0)
			{
				entryEnd += (numAttributeValues - 1) * entry.strideSize + attributeSize(entry);
			}

			attributeStorageSize = std::max(attributeStorageSize, entryEnd);
		}
	}
	else
	{
		std::size_t vertexStrideSize = 0;
		std::size_t faceStrideSize = 0;
		for(AttributeMask entryIndex = 0; entryIndex < numEntries; ++entryIndex)
		{
			Entry& entry = compactEntries[entryIndex];
			if(entry.domain == EAttributeDomain::Vertex)
			{
				entry.u_strideOffset = vertexStrideSize;
				vertexStrideSize += attributeSize(entry);
			}
			else
			{
				entry.u_strideOffset = faceStrideSize;
				faceStrideSize += attributeSize(entry);
			}
		}

		const std::size_t vertexStorageSize = numVertices * vertexStrideSize;
		const std::size_t faceStorageSize = numFaces * faceStrideSize;
		attributeStorageSize = vertexStorageSize + faceStorageSize;

		for(AttributeMask entryIndex = 0; entryIndex < numEntries; ++entryIndex)
		{
			Entry& entry = compactEntries[entryIndex];
			if(entry.domain == EAttributeDomain::Vertex)
			{
				entry.strideSize = vertexStrideSize;
			}
			else
			{
				// Put face attributes after vertex storage region
				entry.u_strideOffset += vertexStorageSize;
				entry.strideSize = faceStrideSize;
			}
		}
	}

	const std::size_t alignment = byteBufferAlignment();
	const std::size_t storageOffset = attributeStorageOffset(numEntries, alignment);
	auto byteBuffer = makeByteBuffer(storageOffset + attributeStorageSize, alignment);

	// Construct the final entry table and set buffer offset information

	Entry* const allocatedEntries = start_implicit_lifetime_as_array<Entry>(byteBuffer.get(), numEntries);
	std::byte* const attributeStorage = byteBuffer.get() + storageOffset;
	for(AttributeMask entryIndex = 0; entryIndex < numEntries; ++entryIndex)
	{
		const std::size_t attributeOffset = compactEntries[entryIndex].u_strideOffset;
		Entry* const entry = std::construct_at(allocatedEntries + entryIndex, compactEntries[entryIndex]);
		entry->u_attributeBuffer = attributeStorage + attributeOffset;
	}

	m_byteBuffer = std::move(byteBuffer);
#if PH_DEBUG
	m_isAttributeAllocated = true;
#endif

	if(attributeStorageSize == 0)
	{
		PH_LOG(IndexedAttributeBuffer, Warning, "Allocated buffer with 0 size.");
	}

	return IndexedAttributeBufferWriter(
		*this,
		attributeStorageSize,
		numVertices,
		numFaces);
}

math::Vector3R IndexedAttributeBuffer::getAttribute(
	const EPrimitiveAttribute attribute,
	const std::size_t index,
	uint32* const out_customBits) const
{
	std::array<uint32, 1> customBits;
	const auto values = getAttribute(
		attribute,
		std::array<std::size_t, 1>{index},
		out_customBits ? &customBits : nullptr);

	if(out_customBits)
	{
		*out_customBits = customBits[0];
	}

	return values[0];
}

IndexedAttributeBufferWriter::IndexedAttributeBufferWriter(
	IndexedAttributeBuffer& buffer,
	const std::size_t attributeStorageSize,
	const std::size_t numVertices,
	const std::size_t numFaces)
	: m_buffer(buffer)
	, m_attributeStorageSize(attributeStorageSize)
	, m_numVertices(numVertices)
	, m_numFaces(numFaces)
{}

void IndexedAttributeBufferWriter::setAttribute(
	const EPrimitiveAttribute attribute,
	const std::size_t index,
	const math::Vector3R& value,
	const uint32* const customBits)
{
	if(!m_buffer.hasEntry(attribute))
	{
		throw_formatted<InvalidArgumentException>(
			"Setting value to an empty primitive attribute {}.",
			enum_to_value(attribute));
	}

	const IndexedAttributeBuffer::Entry& entry = m_buffer.getEntry(attribute);

	const std::size_t numAttributeValues = entry.domain == EAttributeDomain::Vertex
		? numVertices() : numFaces();
	PH_ASSERT_LT(index, numAttributeValues);

	const uint32 customBitsValue = customBits ? *customBits : 0;
#if PH_DEBUG
	if(entry.element == EAttributeElement::OctahedralUnitVec3_31_CustomBits_1)
	{
		PH_ASSERT_LE(customBitsValue, 1);
	}
	else
	{
		PH_ASSERT_EQ(customBitsValue, 0);
	}
#endif

	std::byte* const bufferPtr = entry.u_attributeBuffer + index * entry.strideSize;
	switch(entry.element)
	{
	case EAttributeElement::Float32:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			const auto element = static_cast<float32>(value[ei]);
			std::memcpy(bufferPtr + ei * 4, &element, 4);
		}
		break;

	case EAttributeElement::Float16:
		for(std::size_t ei = 0; ei < entry.numElements; ++ei)
		{
			const uint16 fp16Bits = math::fp32_to_fp16_bits(static_cast<float32>(value[ei]));
			std::memcpy(bufferPtr + ei * 2, &fp16Bits, 2);
		}
		break;

	case EAttributeElement::Int32:
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

	case EAttributeElement::Int16:
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

	case EAttributeElement::OctahedralUnitVec3_32:
		{
			const math::Vector2R encodedVal = math::octahedron_unit_vector_encode(value);

			const math::TVector2<uint16> encodedBits(
				math::quantize_normalized_float<uint16>(encodedVal.x()),
				math::quantize_normalized_float<uint16>(encodedVal.y()));

			std::memcpy(bufferPtr + 0 * 2, &encodedBits.x(), 2);
			std::memcpy(bufferPtr + 1 * 2, &encodedBits.y(), 2);
		}
		break;

	case EAttributeElement::OctahedralUnitVec3_24:
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

	case EAttributeElement::OctahedralUnitVec3_31_CustomBits_1:
		{
			const math::Vector2R encodedVal = math::octahedron_unit_vector_encode(value);

			const math::TVector2<uint32> encodedBits(
				static_cast<uint32>(std::round(encodedVal.x() * 65535.0_r)),
				static_cast<uint32>(std::round(encodedVal.y() * 32767.0_r)));

			PH_ASSERT_LE(encodedBits.x(), 65535);
			PH_ASSERT_LE(encodedBits.y(), 32767);

			const uint32 packedBits = encodedBits.x() | (encodedBits.y() << 16) | (customBitsValue << 31);
			std::memcpy(bufferPtr, &packedBits, sizeof(packedBits));
		}
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}
}

void IndexedAttributeBufferWriter::setData(
	const std::byte* const srcBytes,
	const std::size_t numBytes,
	const std::size_t dstOffset)
{
	PH_ASSERT(srcBytes);

	if(dstOffset > m_attributeStorageSize || numBytes > m_attributeStorageSize - dstOffset)
	{
		throw_formatted<InvalidArgumentException>(
			"Copying {} bytes will overflow the attribute buffer (buffer-size: {} bytes, buffer-offset: {}).",
			numBytes, m_attributeStorageSize, dstOffset);
	}

	std::memcpy(getData() + dstOffset, srcBytes, numBytes);
}

auto IndexedAttributeBuffer::getAttributeDeclaration(const EPrimitiveAttribute attribute) const
-> AttributeDeclaration
{
	PH_ASSERT(m_isAttributeAllocated);

	if(!hasEntry(attribute))
	{
		return AttributeDeclaration();
	}

	const Entry& entry = getEntry(attribute);

	AttributeDeclaration declaration;
	declaration.strideOffset = lossless_integer_cast<std::size_t>(entry.u_attributeBuffer - getData());
	declaration.strideSize = entry.strideSize;
	declaration.element = entry.element;
	declaration.domain = entry.domain;
	declaration.numElements = entry.numElements;
	declaration.shouldNormalize = entry.shouldNormalize;

	return declaration;
}

std::size_t IndexedAttributeBuffer::attributeSize(const Entry& entry)
{
	switch(entry.element)
	{
	case EAttributeElement::Float32:
	case EAttributeElement::Int32:
		return 4 * entry.numElements;

	case EAttributeElement::Float16:
	case EAttributeElement::Int16:
		return 2 * entry.numElements;

	case EAttributeElement::OctahedralUnitVec3_32:
	case EAttributeElement::OctahedralUnitVec3_31_CustomBits_1:
		return 4;

	case EAttributeElement::OctahedralUnitVec3_24:
		return 3;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0;
	}
}

void IndexedAttributeBuffer::ensureConsistentAttributeLayout() const
{
	PH_ASSERT(!m_isAttributeAllocated);

	bool hasAutomaticLayout = false;
	bool hasCustomLayout = false;
	for(AttributeMask attributeIndex = 0; attributeIndex < MAX_ENTRIES; ++attributeIndex)
	{
		const auto attribute = static_cast<EPrimitiveAttribute>(attributeIndex);
		if(hasEntry(attribute))
		{
			const bool usesCustomLayout = getDeclaredEntry(attribute).hasStrideInfo();
			hasCustomLayout |= usesCustomLayout;
			hasAutomaticLayout |= !usesCustomLayout;
		}
	}

	if(hasAutomaticLayout && hasCustomLayout)
	{
		throw InvalidArgumentException(
			"Inconsistent stride info detected. Attributes must all use automatic stride "
			"size/offset (AoS) or all with custom stride size/offset.");
	}
}

std::size_t IndexedAttributeBuffer::byteBufferAlignment()
{
	// Only `Entry` is directly referenced and not copied from buffer
	return std::max(os::get_L1_cache_line_size_in_bytes(), alignof(Entry));
}

std::size_t IndexedAttributeBuffer::refineByteBufferSize(const std::size_t requiredSize, const std::size_t alignment)
{
	const std::size_t nonEmptySize = std::max(requiredSize, alignment);
	return math::next_power_of_2_multiple(nonEmptySize, alignment);
}

auto IndexedAttributeBuffer::makeByteBuffer(const std::size_t requiredSize, const std::size_t alignment)
-> TAlignedMemoryUniquePtr<std::byte>
{
	const std::size_t allocationSize = refineByteBufferSize(requiredSize, alignment);
	auto byteBuffer = make_aligned_memory<std::byte>(allocationSize, alignment);
	if(!byteBuffer)
	{
		throw std::bad_alloc{};
	}

	return byteBuffer;
}

}// end namespace ph
