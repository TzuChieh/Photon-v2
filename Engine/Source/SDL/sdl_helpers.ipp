#pragma once

#include "SDL/sdl_helpers.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "SDL/Tokenizer.h"
#include "SDL/sdl_traits.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/ISdlResource.h"
#include "Utility/string_utils.h"

#include <type_traits>

namespace ph::sdl
{

template<typename FloatType>
inline FloatType load_float(const std::string_view sdlFloatStr)
{
	try
	{
		return string_utils::parse_float<FloatType>(sdlFloatStr);
	}
	catch(const std::exception& e)
	{
		throw SdlLoadError("on loading floating-point value -> " + std::string(e.what()));
	}
}

template<typename IntType>
inline IntType load_int(const std::string_view sdlIntStr)
{
	try
	{
		return string_utils::parse_int<IntType>(sdlIntStr);
	}
	catch(const std::exception& e)
	{
		throw SdlLoadError("on loading integer value -> " + std::string(e.what()));
	}
}

template<typename NumberType>
inline NumberType load_number(const std::string_view sdlNumberStr)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		return load_float<NumberType>(sdlNumberStr);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		return load_int<NumberType>(sdlNumberStr);
	}
}

inline real load_real(const std::string_view sdlRealStr)
{
	return load_float<real>(sdlRealStr);
}

inline integer load_integer(const std::string_view sdlIntegerStr)
{
	return load_int<integer>(sdlIntegerStr);
}

inline void save_real(const real value, std::string* const out_str)
{
	save_float<real>(value, out_str);
}

inline void save_integer(const integer value, std::string* const out_str)
{
	save_int<integer>(value, out_str);
}

template<typename FloatType>
inline void save_float(const FloatType value, std::string* const out_str, const std::size_t maxChars)
{
	try
	{
		string_utils::stringify_number(value, out_str, maxChars);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving floating-point value -> " + std::string(e.what()));
	}
}

template<typename IntType>
inline void save_int(const IntType value, std::string* const out_str, const std::size_t maxChars)
{
	try
	{
		string_utils::stringify_number(value, out_str, maxChars);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving integer value -> " + std::string(e.what()));
	}
}

template<typename NumberType>
inline void save_number(const NumberType value, std::string* const out_str, const std::size_t maxChars)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		save_float<NumberType>(value, out_str, maxChars);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		save_int<NumberType>(value, out_str, maxChars);
	}
}

template<typename Element>
inline math::TVector2<Element> load_vector2(const std::string& sdlVector2Str)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlVector2Str, tokens);

		// 2 input values form a vec2 exactly
		if(tokens.size() == 2)
		{
			return math::TVector2<Element>(
				load_number<Element>(tokens[0]),
				load_number<Element>(tokens[1]));
		}
		// 1 input value results in vec2 filled with the same value
		else if(tokens.size() == 1)
		{
			return math::TVector2<Element>(
				load_number<Element>(tokens[0]));
		}
		else
		{
			throw SdlLoadError(
				"invalid Vector2 representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Vector2 -> " + e.whatStr());
	}
}

template<typename NumberType>
inline std::vector<NumberType> load_number_array(const std::string& sdlNumberArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> arrayTokens;
		tokenizer.tokenize(sdlNumberArrayStr, arrayTokens);

		std::vector<NumberType> numberArray(arrayTokens.size());
		for(std::size_t i = 0; i < numberArray.size(); ++i)
		{
			numberArray[i] = load_number<NumberType>(arrayTokens[i]);
		}

		return numberArray;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing number array -> " + e.whatStr());
	}
}

template<typename Element>
inline void save_vector2(const math::TVector2<Element>& value, std::string* const out_str)
{
	PH_ASSERT(out_str);

	try
	{
		if(value.x() == value.y())
		{
			save_number<Element>(value.x(), out_str);
		}
		else
		{
			out_str->clear();

			std::string savedElement;

			(*out_str) += '"';
			save_number<Element>(value.x(), &savedElement);
			(*out_str) += savedElement;
			(*out_str) += ' ';
			save_number<Element>(value.y(), &savedElement);
			(*out_str) += savedElement;
			(*out_str) += '"';
		}
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Vector2 -> " + e.whatStr());
	}
}

template<typename NumberType>
inline void save_number_array(const std::vector<NumberType>& values, std::string* const out_str)
{
	PH_ASSERT(out_str);
		
	out_str->clear();

	try
	{
		(*out_str) += '"';

		std::string savedNumber;
		for(const auto& value : values)
		{
			save_number<NumberType>(value, &savedNumber);
			(*out_str) += savedNumber;
			(*out_str) += ' ';
		}

		(*out_str) += '"';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving number array -> " + e.whatStr());
	}
}

template<typename T>
inline constexpr ESdlTypeCategory category_of()
{
	if constexpr(CSdlResource<T> && CHasStaticSdlCategoryInfo<T>)
	{
		return T::CATEGORY;
	}
	else
	{
		return ESdlTypeCategory::Unspecified;
	}
}

template<std::integral IntType>
inline constexpr ESdlDataType int_type_of()
{
	if constexpr(CSame<IntType, bool>)
	{
		return ESdlDataType::Bool;
	}
	else if constexpr(CSame<IntType, int8>)
	{
		return ESdlDataType::Int8;
	}
	else if constexpr(CSame<IntType, uint8>)
	{
		return ESdlDataType::UInt8;
	}
	else if constexpr(CSame<IntType, int16>)
	{
		return ESdlDataType::Int16;
	}
	else if constexpr(CSame<IntType, uint16>)
	{
		return ESdlDataType::UInt16;
	}
	else if constexpr(CSame<IntType, int32>)
	{
		return ESdlDataType::Int32;
	}
	else if constexpr(CSame<IntType, uint32>)
	{
		return ESdlDataType::UInt32;
	}
	else if constexpr(CSame<IntType, int64>)
	{
		return ESdlDataType::Int64;
	}
	else
	{
		static_assert(CSame<IntType, uint64>);
		return ESdlDataType::UInt64;
	}
}

template<std::floating_point FloatType>
inline constexpr ESdlDataType float_type_of()
{
	if constexpr(CSame<FloatType, float32>)
	{
		return ESdlDataType::Float32;
	}
	else
	{
		static_assert(CSame<FloatType, float64>);
		return ESdlDataType::Float64;
	}
}

template<CNumber NumberType>
inline constexpr ESdlDataType number_type_of()
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		return sdl::float_type_of<NumberType>();
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);
		return sdl::int_type_of<NumberType>();
	}
}

template<typename T>
inline constexpr ESdlDataType resource_type_of()
{
	constexpr ESdlTypeCategory CATEGORY = sdl::category_of<T>();

	if constexpr(CATEGORY == ESdlTypeCategory::Ref_Geometry)
	{
		return ESdlDataType::Geometry;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Material)
	{
		return ESdlDataType::Material;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Motion)
	{
		return ESdlDataType::Motion;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_LightSource)
	{
		return ESdlDataType::LightSource;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Actor)
	{
		return ESdlDataType::Actor;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Image)
	{
		return ESdlDataType::Image;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_FrameProcessor)
	{
		return ESdlDataType::FrameProcessor;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Observer)
	{
		return ESdlDataType::Observer;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_SampleSource)
	{
		return ESdlDataType::SampleSource;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Visualizer)
	{
		return ESdlDataType::Visualizer;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Option)
	{
		return ESdlDataType::Option;
	}
	else if constexpr(CATEGORY == ESdlTypeCategory::Ref_Object)
	{
		return ESdlDataType::Object;
	}
	else
	{
		static_assert(CATEGORY == ESdlTypeCategory::Unspecified);

		return ESdlDataType::None;
	}
}

template<typename DstType, typename SrcType>
inline DstType* cast_to(SrcType* const srcResource)
{
	// `SrcType` and `DstType` are both possibly cv-qualified

	static_assert(std::is_base_of_v<ISdlResource, SrcType>,
		"Source resource must derive from ISdlResource.");

	static_assert(std::is_base_of_v<ISdlResource, DstType>,
		"Casted-to type must derive from ISdlResource.");

	if(srcResource == nullptr)
	{
		throw SdlException("source resource is empty");
	}

	DstType* const dstResource = dynamic_cast<DstType*>(srcResource);
	if(dstResource == nullptr)
	{
		throw_formatted<SdlException>(
			"type cast error: source resource cannot be casted to the specified type (resource ID: {})",
			srcResource->getId());
	}

	return dstResource;
}

}// end namespace ph::sdl
