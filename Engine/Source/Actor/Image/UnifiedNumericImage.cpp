#include "Actor/Image/UnifiedNumericImage.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/TVector4.h"
#include "Actor/actor_exceptions.h"
#include "Utility/string_utils.h"
#include "Core/Texture/TSwizzledTexture.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "Actor/Image/ConstantImage.h"
#include "Core/Texture/constant_textures.h"

#include <utility>
#include <format>
#include <limits>
#include <array>
#include <string_view>
#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(UnifiedNumericImage, Image);

UnifiedNumericImage::UnifiedNumericImage() :

	Image(),

	m_image            (nullptr),
	m_swizzleSubscripts(),
	m_constant         (0.0)
{}

namespace
{

// Char set for different types of swizzle subscripts. Specified in full width, identity ordering.
// Note: Currently all chars are unique; if not, the logics in make_char_to_index_table() need to be changed
inline constexpr std::array<std::string_view, 4> SWIZZLE_CHAR_SETS = 
{
	// Cartesian coordinates
	"xyzw",

	// color
	"rgba",

	// image/texture coordinates
	"stpq",

	// hex-based subscripts for wide vector types
	"0123456789ABCDEF"
};

using SwizzleTableType = std::array<uint8, std::numeric_limits<char>::max()>;

// A table that maps swizzle subscript to array index
inline constexpr SwizzleTableType make_char_to_index_table()
{
	SwizzleTableType table;
	table.fill(std::numeric_limits<uint8>::max());

	for(std::size_t setIdx = 0; setIdx < SWIZZLE_CHAR_SETS.size(); ++setIdx)
	{
		const auto charSet = SWIZZLE_CHAR_SETS[setIdx];
		for(uint8 i = 0; i < charSet.size(); ++i)
		{
			table[charSet[i]] = i;
		}
	}

	return table;
}

inline uint8 subscript_to_index(const char ch)
{
	static const auto CHAR_TO_INDEX = make_char_to_index_table();

	return CHAR_TO_INDEX[ch];
}

inline std::size_t used_swizzle_char_set(const std::string_view swizzleSubscripts)
{
	// An empty subscript will always match and return true
	auto isSubscriptsInCharSet = [swizzleSubscripts](const std::string_view charSet)
	{
		for(const char subscriptChar : swizzleSubscripts)
		{
			if(charSet.find(subscriptChar) == std::string_view::npos)
			{
				return false;
			}
		}
		return true;
	};

	for(std::size_t setIdx = 0; setIdx < SWIZZLE_CHAR_SETS.size(); ++setIdx)
	{
		const auto charSet = SWIZZLE_CHAR_SETS[setIdx];
		if(isSubscriptsInCharSet(charSet))
		{
			return setIdx;
		}
	}

	return static_cast<std::size_t>(-1);
}

// Check if the swizzle subscripts do not actually swizzle anything
// (empty subscript is also an identity swizzle)
inline bool is_identity_swizzle(const std::string_view swizzleSubscripts)
{
	std::string_view charSet;
	if(const auto setIdx = used_swizzle_char_set(swizzleSubscripts); setIdx < SWIZZLE_CHAR_SETS.size())
	{
		charSet = SWIZZLE_CHAR_SETS[setIdx];
	}
	else
	{
		return false;
	}

	// If the subscripts is wider than the swizzle char set, it is not identity
	if(swizzleSubscripts.size() > charSet.size())
	{
		return false;
	}

	// Subscripts must be a prefix of the swizzle char set to be identity
	PH_ASSERT_LE(swizzleSubscripts.size(), charSet.size());
	return std::equal(swizzleSubscripts.begin(), swizzleSubscripts.end(), charSet.begin());
}

// Create an index map by mapping input swizzle subscript to array index. If `N` is greater than the number
// of subscripts, remaining indices are mapped identically.
template<std::size_t N>
inline std::array<uint8, N> to_texture_swizzle_map(const std::string_view swizzleSubscripts)
{
	if(used_swizzle_char_set(swizzleSubscripts) == static_cast<std::size_t>(-1))
	{
		throw ActorCookException(std::format(
			"Specified swizzle subscripts ({}) is invalid.",
			swizzleSubscripts.size()));
	}

	// Fill identitically first
	std::array<uint8, N> swizzleMap;
	for(std::size_t i = 0; i < N; ++i)
	{
		swizzleMap[i] = static_cast<uint8>(i);
	}

	for(std::size_t i = 0; i < std::min(N, swizzleSubscripts.size()); ++i)
	{
		swizzleMap[i] = subscript_to_index(swizzleSubscripts[i]);

		// Should not have mapped any unsupported subscript
		PH_ASSERT_NE(swizzleMap[i], std::numeric_limits<uint8>::max());
	}

	// Check if the swizzle map will overflow the array size of numeric texture
	for(const auto idxOfInput : swizzleMap)
	{
		if(idxOfInput >= Image::ARRAY_SIZE)
		{
			throw ActorCookException("Swizzle subscripts overflow input array.");
		}
	}

	return swizzleMap;
}

// Create a fixed-size index map by mapping input swizzle subscript to array index. Number of subscripts
// must be equal to `N`.
template<std::size_t N>
inline std::array<uint8, N> to_exact_texture_swizzle_map(const std::string_view swizzleSubscripts)
{
	if(N != swizzleSubscripts.size())
	{
		throw ActorCookException(std::format(
			"Mismatched number of subscripts ({}) to specified number of components ({}).", 
			swizzleSubscripts.size(), N));
	}

	return to_texture_swizzle_map<N>(swizzleSubscripts);
}

}// end anonymous namespace

std::shared_ptr<TTexture<Image::Array>> UnifiedNumericImage::genNumericTexture(
	CookingContext& ctx)
{
	if(m_swizzleSubscripts.size() > Image::ARRAY_SIZE)
	{
		throw ActorCookException(std::format(
			"Input swizzle subscripts has more elements ({}) than the max number ({}) swizzler can handle.",
			m_swizzleSubscripts.size(), Image::ARRAY_SIZE));
	}

	if(m_image)
	{
		if(is_identity_swizzle(m_swizzleSubscripts))
		{
			return m_image->genNumericTexture(ctx);
		}
		else
		{
			const auto swizzleMap = to_texture_swizzle_map<Image::ARRAY_SIZE>(m_swizzleSubscripts);
			return std::make_shared<TSwizzledTexture<Image::Array, Image::Array, Image::ARRAY_SIZE>>(
				m_image->genNumericTexture(ctx), swizzleMap);
		}
	}
	else
	{
		return std::make_shared<TConstantTexture<Image::Array>>(m_constant);
	}
}

std::shared_ptr<TTexture<math::Spectrum>> UnifiedNumericImage::genColorTexture(
	CookingContext& ctx)
{
	if(!m_swizzleSubscripts.empty())
	{
		// As spectrum can be any type from tristimulus to spectral distribution, their number of
		// components can vary. Swizzling such a type would be inconsistent if the same scene file
		// is used for different spectrum type configuration.
		//
		throw ActorCookException(std::format(
			"Swizzling (subscript {}) is forbidden for color texture from unified numeric image.",
			m_swizzleSubscripts));
	}

	if(m_image)
	{
		return m_image->genColorTexture(ctx);
	}
	else
	{
		return ConstantImage(
			math::Vector3R(static_cast<real>(m_constant[0]), static_cast<real>(m_constant[1]), static_cast<real>(m_constant[2])),
			math::EColorSpace::Linear_sRGB).genColorTexture(ctx);
	}
}

std::shared_ptr<TTexture<real>> UnifiedNumericImage::genRealTexture(CookingContext& ctx)
{
	if(m_image)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<1>(m_swizzleSubscripts);

		auto numericArrayToReal = [mappedIndex = swizzleMap[0]](const Image::Array& inputValue)
		{
			return static_cast<real>(inputValue[mappedIndex]);
		};

		return std::make_shared<TUnaryTextureOperator<Image::Array, real, decltype(numericArrayToReal)>>(
			m_image->genNumericTexture(ctx), std::move(numericArrayToReal));
	}
	else
	{
		return std::make_shared<TConstantTexture<real>>(static_cast<real>(m_constant[0]));
	}
}

std::shared_ptr<TTexture<math::Vector2R>> UnifiedNumericImage::genVector2RTexture(CookingContext& ctx)
{
	if(m_image)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<2>(m_swizzleSubscripts);
		return std::make_shared<TSwizzledTexture<Image::Array, math::Vector2R, 2>>(
			m_image->genNumericTexture(ctx), swizzleMap);
	}
	else
	{
		return std::make_shared<TConstantTexture<math::Vector2R>>(
			math::Vector2R(static_cast<real>(m_constant[0]), static_cast<real>(m_constant[1])));
	}
}

std::shared_ptr<TTexture<math::Vector3R>> UnifiedNumericImage::genVector3RTexture(CookingContext& ctx)
{
	if(m_image)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<3>(m_swizzleSubscripts);
		return std::make_shared<TSwizzledTexture<Image::Array, math::Vector3R, 3>>(
			m_image->genNumericTexture(ctx), swizzleMap);
	}
	else
	{
		return std::make_shared<TConstantTexture<math::Vector3R>>(
			math::Vector3R(static_cast<real>(m_constant[0]), static_cast<real>(m_constant[1]), static_cast<real>(m_constant[2])));
	}
}

std::shared_ptr<TTexture<math::Vector4R>> UnifiedNumericImage::genVector4RTexture(CookingContext& ctx)
{
	if(m_image)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<4>(m_swizzleSubscripts);
		return std::make_shared<TSwizzledTexture<Image::Array, math::Vector4R, 4>>(
			m_image->genNumericTexture(ctx), swizzleMap);
	}
	else
	{
		return std::make_shared<TConstantTexture<math::Vector4R>>(
			math::Vector4R(static_cast<real>(m_constant[0]), static_cast<real>(m_constant[1]), static_cast<real>(m_constant[2]), static_cast<real>(m_constant[3])));
	}
}

UnifiedNumericImage& UnifiedNumericImage::setImage(std::shared_ptr<Image> image)
{
	m_image = std::move(image);
	return *this;
}

UnifiedNumericImage& UnifiedNumericImage::setSwizzleSubscripts(std::string swizzleSubscripts)
{
	m_swizzleSubscripts = std::move(swizzleSubscripts);
	return *this;
}

UnifiedNumericImage& UnifiedNumericImage::setConstant(const float64* const constantData, const std::size_t dataSize)
{
	PH_ASSERT(constantData);

	if(dataSize > m_constant.size())
	{
		PH_LOG_WARNING(UnifiedNumericImage,
			"Data loss detected: setting constant with {} elements while the max allowed number is {}",
			dataSize, m_constant.size());
	}

	m_constant.set(0.0);
	for(std::size_t i = 0; i < std::min(m_constant.size(), dataSize); ++i)
	{
		m_constant[i] = constantData[i];
	}

	return *this;
}

}// end namespace ph
