#pragma once

#include "SDL/sdl_helpers.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/TVector4.h"
#include "Math/TQuaternion.h"
#include "SDL/Tokenizer.h"
#include "SDL/sdl_traits.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/ISdlResource.h"

#include <Common/assertion.h>
#include <Common/Utility/string_utils.h>

#include <type_traits>
#include <algorithm>
#include <exception>

namespace ph::sdl
{

template<typename FloatType>
inline FloatType load_float(std::string_view sdlFloatStr)
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
inline IntType load_int(std::string_view sdlIntStr)
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
inline NumberType load_number(std::string_view sdlNumberStr)
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

template<typename NumberType, std::size_t EXTENT>
inline void load_numbers(std::string_view sdlNumbersStr, TSpan<NumberType, EXTENT> out_numbers)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	const auto N = out_numbers.size();
	PH_ASSERT_GT(N, 0);

	try
	{
		// TODO: use view
		std::vector<std::string> tokens;
		tokenizer.tokenize(std::string(sdlNumbersStr), tokens);

		// N input values form a N-tuple exactly
		if(tokens.size() == N)
		{
			for(std::size_t ni = 0; ni < N; ++ni)
			{
				out_numbers[ni] = load_number<NumberType>(tokens[ni]);
			}
		}
		// 1 input value results in the N-tuple filled with the same value
		else if(tokens.size() == 1)
		{
			std::fill(out_numbers.begin(), out_numbers.end(), load_number<NumberType>(tokens[0]));
		}
		else
		{
			throw_formatted<SdlLoadError>(
				"invalid number of tokens: {}", tokens.size());
		}
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlLoadError>(
			"on parsing {}-tuple -> {}", out_numbers.size(), e.whatStr());
	}
}

inline real load_real(std::string_view sdlRealStr)
{
	return load_float<real>(sdlRealStr);
}

inline integer load_integer(std::string_view sdlIntegerStr)
{
	return load_int<integer>(sdlIntegerStr);
}

template<typename Element>
inline math::TVector2<Element> load_vector2(std::string_view sdlVec2Str)
{
	try
	{
		math::TVector2<Element> vec2;
		load_numbers(sdlVec2Str, vec2.toSpan());
		return vec2;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Vector2 -> " + e.whatStr());
	}
}

template<typename Element>
inline math::TVector3<Element> load_vector3(std::string_view sdlVec3Str)
{
	try
	{
		math::TVector3<Element> vec3;
		load_numbers(sdlVec3Str, vec3.toSpan());
		return vec3;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Vector3 -> " + e.whatStr());
	}
}

template<typename Element>
inline math::TVector4<Element> load_vector4(std::string_view sdlVec4Str)
{
	try
	{
		math::TVector4<Element> vec4;
		load_numbers(sdlVec4Str, vec4.toSpan());
		return vec4;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Vector4 -> " + e.whatStr());
	}
}

template<typename Element>
inline math::TQuaternion<Element> load_quaternion(std::string_view sdlQuatStr)
{
	try
	{
		math::TQuaternion<Element> quat;
		load_numbers(sdlQuatStr, quat.toSpan());
		return quat;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Quaternion -> " + e.whatStr());
	}
}

template<typename NumberType>
inline std::vector<NumberType> load_number_array(std::string_view sdlNumberArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		// TODO: use view
		std::vector<std::string> arrayTokens;
		tokenizer.tokenize(std::string(sdlNumberArrayStr), arrayTokens);

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
inline std::vector<math::TVector3<Element>> load_vector3_array(std::string_view sdlVec3ArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'"', '"'}});

	try
	{
		// TODO: use view
		std::vector<std::string> vec3Tokens;
		tokenizer.tokenize(std::string(sdlVec3ArrayStr), vec3Tokens);

		std::vector<math::TVector3<Element>> vec3Array(vec3Tokens.size());
		for(std::size_t i = 0; i < vec3Array.size(); ++i)
		{
			vec3Array[i] = load_vector3<Element>(vec3Tokens[i]);
		}

		return vec3Array;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Vector3 array -> " + e.whatStr());
	}
}

inline void save_real(const real value, std::string& out_str)
{
	save_float<real>(value, out_str);
}

inline void save_integer(const integer value, std::string& out_str)
{
	save_int<integer>(value, out_str);
}

template<typename FloatType>
inline void save_float(const FloatType value, std::string& out_str)
{
	try
	{
		string_utils::stringify_number(value, out_str, 32);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving floating-point value -> " + std::string(e.what()));
	}
}

template<typename IntType>
inline void save_int(const IntType value, std::string& out_str)
{
	try
	{
		string_utils::stringify_number(value, out_str, 32);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving integer value -> " + std::string(e.what()));
	}
}

template<typename NumberType>
inline void save_number(const NumberType value, std::string& out_str)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		save_float(value, out_str);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		save_int(value, out_str);
	}
}

template<typename NumberType, std::size_t EXTENT>
inline void save_numbers(
	TSpanView<NumberType, EXTENT> numbers,
	std::string& out_str)
{
	const auto N = numbers.size();
	PH_ASSERT_GT(N, 0);

	try
	{
		// If all values are equal, we can save it as one value
		if(std::equal(numbers.begin() + 1, numbers.end(), numbers.begin()))
		{
			save_number<NumberType>(numbers[0], out_str);
		}
		else
		{
			save_number<NumberType>(numbers[0], out_str);
			for(std::size_t ni = 1; ni < N; ++ni)
			{
				out_str += ' ';
				save_number<NumberType>(numbers[ni], out_str);
			}
		}
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlSaveError>(
			"on saving {}-tuple -> {}", numbers.size(), e.whatStr());
	}
}

template<typename Element>
inline void save_vector2(const math::TVector2<Element>& value, std::string& out_str)
{
	try
	{
		out_str += '"';
		save_numbers(value.toView(), out_str);
		out_str += '"';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Vector2 -> " + e.whatStr());
	}
}

template<typename Element>
inline void save_vector3(const math::TVector3<Element>& value, std::string& out_str)
{
	try
	{
		out_str += '"';
		save_numbers(value.toView(), out_str);
		out_str += '"';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Vector3 -> " + e.whatStr());
	}
}

template<typename Element>
inline void save_vector4(const math::TVector4<Element>& value, std::string& out_str)
{
	try
	{
		out_str += '"';
		save_numbers(value.toView(), out_str);
		out_str += '"';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Vector4 -> " + e.whatStr());
	}
}

template<typename Element>
inline void save_quaternion(const math::TQuaternion<Element>& value, std::string& out_str)
{
	try
	{
		out_str += '"';
		save_numbers(value.toView(), out_str);
		out_str += '"';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Quaternion -> " + e.whatStr());
	}
}

template<typename NumberType>
inline void save_number_array(TSpanView<NumberType> values, std::string& out_str)
{
	try
	{
		out_str += '"';
		save_numbers(values, out_str);
		out_str += '"';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving number array -> " + e.whatStr());
	}
}

template<typename Element>
inline void save_vector3_array(TSpanView<math::TVector3<Element>> values, std::string& out_str)
{
	try
	{
		out_str += '{';

		if(!values.empty())
		{
			save_vector3(values[0], out_str);
		}

		for(std::size_t vi = 1; vi < values.size(); ++vi)
		{
			out_str += ' ';
			save_vector3(values[vi], out_str);
		}

		out_str += '}';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Vector3 array -> " + e.whatStr());
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
