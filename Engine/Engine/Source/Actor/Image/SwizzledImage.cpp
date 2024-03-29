#include "Actor/Image/SwizzledImage.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/TVector4.h"
#include "Actor/Basic/exceptions.h"
#include "Core/Texture/TSwizzledTexture.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "Actor/Image/ConstantImage.h"
#include "Core/Texture/constant_textures.h"
#include "Math/Color/color_spaces.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/Utility/string_utils.h>

#include <cstddef>
#include <utility>
#include <limits>
#include <array>
#include <string_view>
#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SwizzledImage, Image);

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

// Create an index map by mapping input swizzle subscript to array index. If `N` is greater than the
// number of subscripts, remaining indices are mapped identically.
template<std::size_t N>
inline std::array<uint8, N> to_texture_swizzle_map(const std::string_view swizzleSubscripts)
{
	if(used_swizzle_char_set(swizzleSubscripts) == static_cast<std::size_t>(-1))
	{
		throw_formatted<CookException>(
			"Specified swizzle subscripts ({}) are invalid.", swizzleSubscripts.size());
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
			throw CookException("Swizzle subscripts overflow input array.");
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
		throw_formatted<CookException>(
			"Mismatched number of subscripts ({}) to specified number of components ({}).", 
			swizzleSubscripts.size(), N);
	}

	return to_texture_swizzle_map<N>(swizzleSubscripts);
}

}// end anonymous namespace

std::shared_ptr<TTexture<Image::ArrayType>> SwizzledImage::genNumericTexture(const CookingContext& ctx)
{
	if(m_swizzleSubscripts.size() > Image::ARRAY_SIZE)
	{
		throw_formatted<CookException>(
			"Input swizzle subscripts has more elements ({}) than the max number ({}) swizzler can handle.",
			m_swizzleSubscripts.size(), Image::ARRAY_SIZE);
	}

	if(m_input)
	{
		if(is_identity_swizzle(m_swizzleSubscripts))
		{
			return m_input->genNumericTexture(ctx);
		}
		else
		{
			const auto swizzleMap = to_texture_swizzle_map<Image::ARRAY_SIZE>(m_swizzleSubscripts);
			return std::make_shared<TSwizzledTexture<Image::ArrayType, Image::ArrayType, Image::ARRAY_SIZE>>(
				m_input->genNumericTexture(ctx), swizzleMap);
		}
	}
	else
	{
		return std::make_shared<TConstantTexture<Image::ArrayType>>(Image::ArrayType(0));
	}
}

std::shared_ptr<TTexture<math::Spectrum>> SwizzledImage::genColorTexture(const CookingContext& ctx)
{
	if(!m_input)
	{
		auto constantImage = TSdl<ConstantImage>::make();
		constantImage.setColor(0, math::EColorSpace::Linear_sRGB);
		return constantImage.genColorTexture(ctx);
	}

	if constexpr(math::TColorSpaceDef<math::Spectrum::getColorSpace()>::isTristimulus())
	{
		if(is_identity_swizzle(m_swizzleSubscripts))
		{
			return m_input->genColorTexture(ctx);
		}
		else
		{
			if(m_swizzleSubscripts.size() > 3)
			{
				throw_formatted<CookException>(
					"Too many subscripts ({}) for a tristimulus color texture.", m_swizzleSubscripts);
			}

			switch(m_swizzleSubscripts.size())
			{
			case 1:
				return std::make_shared<TSwizzledTexture<math::Spectrum, math::Spectrum, 1>>(
					m_input->genColorTexture(ctx), 
					to_texture_swizzle_map<1>(m_swizzleSubscripts));

			case 2:
				return std::make_shared<TSwizzledTexture<math::Spectrum, math::Spectrum, 2>>(
					m_input->genColorTexture(ctx),
					to_texture_swizzle_map<2>(m_swizzleSubscripts));

			case 3:
				return std::make_shared<TSwizzledTexture<math::Spectrum, math::Spectrum, 3>>(
					m_input->genColorTexture(ctx),
					to_texture_swizzle_map<3>(m_swizzleSubscripts));

			default:
				PH_ASSERT_UNREACHABLE_SECTION();
				return nullptr;
			}
		}
	}
	else
	{
		if(!m_swizzleSubscripts.empty())
		{
			// As spectral distribution can have any number of components, swizzling such a type
			// would be inconsistent if the same scene file is used for different configurations on 
			// spectral distribution. Also it is very likely the spectrum has more elements than the
			// swizzler can handle. We do not support it for now for simplicity.
			throw_formatted<CookException>(
				"Swizzling (subscript {}) is forbidden for non-tristimulus color texture.", m_swizzleSubscripts);
		}

		return m_input->genColorTexture(ctx);
	}
}

std::shared_ptr<TTexture<real>> SwizzledImage::genRealTexture(const CookingContext& ctx)
{
	if(m_input)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<1>(m_swizzleSubscripts);
		auto numericArrayToReal = [mappedIndex = swizzleMap[0]](const Image::ArrayType& inputValue)
		{
			return static_cast<real>(inputValue[mappedIndex]);
		};

		return std::make_shared<TUnaryTextureOperator<Image::ArrayType, real, decltype(numericArrayToReal)>>(
			m_input->genNumericTexture(ctx), std::move(numericArrayToReal));
	}
	else
	{
		return std::make_shared<TConstantTexture<real>>(0.0_r);
	}
}

std::shared_ptr<TTexture<math::Vector2R>> SwizzledImage::genVector2RTexture(const CookingContext& ctx)
{
	if(m_input)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<2>(m_swizzleSubscripts);
		return std::make_shared<TSwizzledTexture<Image::ArrayType, math::Vector2R, 2>>(
			m_input->genNumericTexture(ctx), swizzleMap);
	}
	else
	{
		return std::make_shared<TConstantTexture<math::Vector2R>>(math::Vector2R(0));
	}
}

std::shared_ptr<TTexture<math::Vector3R>> SwizzledImage::genVector3RTexture(const CookingContext& ctx)
{
	if(m_input)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<3>(m_swizzleSubscripts);
		return std::make_shared<TSwizzledTexture<Image::ArrayType, math::Vector3R, 3>>(
			m_input->genNumericTexture(ctx), swizzleMap);
	}
	else
	{
		return std::make_shared<TConstantTexture<math::Vector3R>>(math::Vector3R(0));
	}
}

std::shared_ptr<TTexture<math::Vector4R>> SwizzledImage::genVector4RTexture(const CookingContext& ctx)
{
	if(m_input)
	{
		const auto swizzleMap = to_exact_texture_swizzle_map<4>(m_swizzleSubscripts);
		return std::make_shared<TSwizzledTexture<Image::ArrayType, math::Vector4R, 4>>(
			m_input->genNumericTexture(ctx), swizzleMap);
	}
	else
	{
		return std::make_shared<TConstantTexture<math::Vector4R>>(math::Vector4R(0));
	}
}

SwizzledImage& SwizzledImage::setInput(std::shared_ptr<Image> input)
{
	m_input = std::move(input);
	return *this;
}

SwizzledImage& SwizzledImage::setSwizzleSubscripts(std::string swizzleSubscripts)
{
	m_swizzleSubscripts = std::move(swizzleSubscripts);
	return *this;
}

Image* SwizzledImage::getInput() const
{
	return m_input.get();
}

std::string_view SwizzledImage::getSwizzleSubscripts() const
{
	return m_swizzleSubscripts;
}

}// end namespace ph
