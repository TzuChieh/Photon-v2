#pragma once

#include "Engine/Core/Intersection/DataStructure/IndexedVertexBuffer.h"
#include "Engine/Math/math.h"
#include "Engine/Math/Geometry/geometry.h"

#include <cstring>
#include <type_traits>

namespace ph
{

template<std::size_t N, std::unsigned_integral Index>
inline std::array<math::Vector3R, N> IndexedVertexBuffer::getAttribute(
	const EVertexAttribute         attribute,
	const std::array<Index, N>&    indices) const
{
	static_assert(N > 0);
	PH_ASSERT(isAllocated());

	std::array<math::Vector3R, N> values;
	if(!hasEntry(attribute))
	{
		values.fill(math::Vector3R(0));
		return values;
	}

	const Entry& entry = getEntry(attribute);
	PH_ASSERT(!entry.isEmpty());

	switch(entry.element)
	{
	case EVertexElement::Float32:
		return loadAttributeValues<EVertexElement::Float32>(entry, indices);

	case EVertexElement::Float16:
		return loadAttributeValues<EVertexElement::Float16>(entry, indices);

	case EVertexElement::Int32:
		return loadAttributeValues<EVertexElement::Int32>(entry, indices);

	case EVertexElement::Int16:
		return loadAttributeValues<EVertexElement::Int16>(entry, indices);

	case EVertexElement::OctahedralUnitVec3_32:
		return loadAttributeValues<EVertexElement::OctahedralUnitVec3_32>(entry, indices);

	case EVertexElement::OctahedralUnitVec3_24:
		return loadAttributeValues<EVertexElement::OctahedralUnitVec3_24>(entry, indices);

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		values.fill(math::Vector3R(0));
		return values;
	}
}

template<EVertexElement Element, std::size_t N, std::unsigned_integral Index>
inline std::array<math::Vector3R, N> IndexedVertexBuffer::loadAttributeValues(
	const Entry&                   entry,
	const std::array<Index, N>&    indices)
{
	constexpr bool canCopyDirectly = Element == EVertexElement::Float32 && std::is_same_v<real, float32>;

	if constexpr(canCopyDirectly)
	{
		switch(entry.numElements)
		{
		case 3:
			return loadAttributeValuesDirectly<3>(entry, indices);

		case 2:
			return loadAttributeValuesDirectly<2>(entry, indices);

		case 1:
			return loadAttributeValuesDirectly<1>(entry, indices);

		default:
			PH_ASSERT_UNREACHABLE_SECTION();
			return {};
		}
	}

	std::array<math::Vector3R, N> values{};
	for(std::size_t vi = 0; vi < N; ++vi)
	{
		const std::byte* const bufferPtr = entry.u_attributeBuffer + indices[vi] * entry.strideSize;
		PH_ASSERT(bufferPtr);

		if constexpr(Element == EVertexElement::Float32)
		{
			for(std::size_t ei = 0; ei < entry.numElements; ++ei)
			{
				float32 element;
				std::memcpy(&element, bufferPtr + ei * sizeof(element), sizeof(element));
				values[vi][ei] = element;
			}
		}
		else if constexpr(Element == EVertexElement::Float16)
		{
			for(std::size_t ei = 0; ei < entry.numElements; ++ei)
			{
				uint16 fp16Bits;
				std::memcpy(&fp16Bits, bufferPtr + ei * sizeof(fp16Bits), sizeof(fp16Bits));
				values[vi][ei] = math::fp16_bits_to_fp32(fp16Bits);
			}
		}
		else if constexpr(Element == EVertexElement::Int32)
		{
			for(std::size_t ei = 0; ei < entry.numElements; ++ei)
			{
				int32 element;
				std::memcpy(&element, bufferPtr + ei * sizeof(element), sizeof(element));

				values[vi][ei] = entry.shouldNormalize
					? math::normalize_integer<real>(element)
					: static_cast<real>(element);
			}
		}
		else if constexpr(Element == EVertexElement::Int16)
		{
			for(std::size_t ei = 0; ei < entry.numElements; ++ei)
			{
				int16 element;
				std::memcpy(&element, bufferPtr + ei * sizeof(element), sizeof(element));

				values[vi][ei] = entry.shouldNormalize
					? math::normalize_integer<real>(element)
					: static_cast<real>(element);
			}
		}
		else if constexpr(Element == EVertexElement::OctahedralUnitVec3_32)
		{
			math::TVector2<uint16> encodedBits;
			std::memcpy(&encodedBits.x(), bufferPtr + 0 * sizeof(uint16), sizeof(uint16));
			std::memcpy(&encodedBits.y(), bufferPtr + 1 * sizeof(uint16), sizeof(uint16));

			const math::Vector2R encodedVal(
				math::normalize_integer<real>(encodedBits.x()),
				math::normalize_integer<real>(encodedBits.y()));

			values[vi] = math::octahedron_unit_vector_decode(encodedVal);
		}
		else
		{
			static_assert(Element == EVertexElement::OctahedralUnitVec3_24);

			// Read 3 bytes (we use only the first 3 bytes of the uint32)
			uint32 packedBits = 0;
			std::memcpy(&packedBits, bufferPtr, 3);

			const math::TVector2<uint32> encodedBits(
				(packedBits & 0x00000FFF),
				(packedBits & 0x00FFF000) >> 12);

			PH_ASSERT_LE(encodedBits.x(), 4096 - 1);
			PH_ASSERT_LE(encodedBits.y(), 4096 - 1);

			const math::Vector2R encodedVal(
				static_cast<real>(encodedBits.x()) / 4095.0_r,
				static_cast<real>(encodedBits.y()) / 4095.0_r);

			values[vi] = math::octahedron_unit_vector_decode(encodedVal);
		}
	}
	return values;
}

template<std::size_t NumElements, std::size_t N, std::unsigned_integral Index>
inline std::array<math::Vector3R, N> IndexedVertexBuffer::loadAttributeValuesDirectly(
	const Entry&                   entry,
	const std::array<Index, N>&    indices)
{
	static_assert(NumElements >= 1 && NumElements <= 3);
	static_assert(std::is_same_v<real, float32>);
	PH_ASSERT(entry.element == EVertexElement::Float32);

	std::array<math::Vector3R, N> values{};
	for(std::size_t vi = 0; vi < N; ++vi)
	{
		const std::byte* const bufferPtr = entry.u_attributeBuffer + indices[vi] * entry.strideSize;
		PH_ASSERT(bufferPtr);

		std::memcpy(values[vi].data(), bufferPtr, NumElements * sizeof(float32));
	}
	return values;
}

}// end namespace ph
