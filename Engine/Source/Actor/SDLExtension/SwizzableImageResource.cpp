#include "Actor/SDLExtension/SwizzableImageResource.h"
#include "Actor/Image/Image.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Actor/actor_exceptions.h"
#include "Utility/string_utils.h"
#include "Core/Texture/TSwizzledTexture.h"
#include "Core/Texture/Function/unary_texture_operators.h"

#include <utility>
#include <format>
#include <limits>
#include <array>
#include <string_view>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SwizzableImageResource, Image);

SwizzableImageResource::SwizzableImageResource(std::shared_ptr<Image> image, std::string swizzleSubscripts) :
	m_image            (std::move(image)),
	m_swizzleSubscripts(std::move(swizzleSubscripts))
{
	PH_ASSERT(m_image);

	if(m_swizzleSubscripts.size() > Image::NUMERIC_ARRAY_SIZE)
	{
		throw ActorCookException(std::format(
			"Input swizzle subscripts has more elements ({}) than the max number ({}) swizzler can handle.",
			m_swizzleSubscripts.size(), Image::NUMERIC_ARRAY_SIZE));
	}
}

namespace
{

using SwizzleTableType = std::array<uint8, std::numeric_limits<char>::max()>;

inline constexpr SwizzleTableType make_char_to_index_table()
{
	SwizzleTableType table;
	table.fill(std::numeric_limits<uint8>::max());

	// rgba subscripts
	table['r'] = 0;
	table['g'] = 1;
	table['b'] = 2;
	table['a'] = 3;

	// xyzw subscripts
	table['x'] = 0;
	table['y'] = 1;
	table['z'] = 2;
	table['w'] = 3;

	// stpq subscripts
	table['s'] = 0;
	table['t'] = 1;
	table['p'] = 2;
	table['q'] = 3;

	// hexadecimal subscripts
	table['0'] = 0;
	table['1'] = 1;
	table['2'] = 2;
	table['3'] = 3;
	table['4'] = 4;
	table['5'] = 5;
	table['6'] = 6;
	table['7'] = 7;
	table['8'] = 8;
	table['9'] = 9;
	table['A'] = 10;
	table['B'] = 11;
	table['C'] = 12;
	table['D'] = 13;
	table['E'] = 14;
	table['F'] = 15;

	return table;
}

template<std::size_t N>
inline std::array<uint8, N> make_texture_swizzle_map(const std::string_view swizzleSubscripts)
{
	constexpr auto CHAR_TO_INDEX = make_char_to_index_table();

	if(N != swizzleSubscripts.size())
	{
		throw ActorCookException(std::format(
			"Mismatched number of subscripts ({}) to specified number of components ({})", 
			swizzleSubscripts.size(), N));
	}

	std::array<uint8, N> swizzleMap;
	for(std::size_t i = 0; i < N; ++i)
	{
		swizzleMap[i] = CHAR_TO_INDEX[swizzleSubscripts[i]];

		if(swizzleMap[i] == std::numeric_limits<uint8>::max())
		{
			throw ActorCookException(std::format(
				"Unsupported swizzle subscript \'{}\' detected.", swizzleSubscripts[i]));
		}
	}
	return swizzleMap;
}

}// end anonymous namespace

std::shared_ptr<TTexture<real>> SwizzableImageResource::genRealTexture(ActorCookingContext& ctx)
{
	const auto swizzleMap = make_texture_swizzle_map<1>(m_swizzleSubscripts);

	auto numericArrayToReal = [mappedIndex = swizzleMap[0]](const Image::NumericArray& inputValue)
	{
		return static_cast<real>(inputValue[mappedIndex]);
	};

	return std::make_shared<TUnaryTextureOperator<Image::NumericArray, real, decltype(numericArrayToReal)>>(
		m_image, std::move(numericArrayToReal));
}

std::shared_ptr<TTexture<math::Vector2R>> SwizzableImageResource::genVector2RTexture(ActorCookingContext& ctx)
{
	const auto swizzleMap = make_texture_swizzle_map<2>(m_swizzleSubscripts);

	return std::make_shared<TSwizzledTexture<Image::NumericArray, math::Vector2R, 2>>(
		m_image, swizzleMap);
}

std::shared_ptr<TTexture<math::Vector3R>> SwizzableImageResource::genVector3RTexture(ActorCookingContext& ctx)
{
	const auto swizzleMap = make_texture_swizzle_map<3>(m_swizzleSubscripts);

	return std::make_shared<TSwizzledTexture<Image::NumericArray, math::Vector3R, 3>>(
		m_image, swizzleMap);
}

std::shared_ptr<TTexture<math::Vector4R>> SwizzableImageResource::genVector4RTexture(ActorCookingContext& ctx)
{
	const auto swizzleMap = make_texture_swizzle_map<4>(m_swizzleSubscripts);

	return std::make_shared<TSwizzledTexture<Image::NumericArray, math::Vector4R, 4>>(
		m_image, swizzleMap);
}

}// end namespace ph
