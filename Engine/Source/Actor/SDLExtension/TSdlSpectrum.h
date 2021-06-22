#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/Quantity/Spectrum.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"

#include <type_traits>
#include <string>
#include <vector>
#include <utility>

namespace ph
{

//template<typename Owner, typename SdlValueType = TSdlValue<Spectrum, Owner>>
//class TSdlSpectrum : public SdlValueType
//{
//	static_assert(std::is_base_of_v<TAbstractSdlValue<Spectrum, Owner>, SdlValueType>,
//		"SdlValueType should be a subclass of TAbstractSdlValue.");
//
//public:
//	template<typename ValueType>
//	inline TSdlSpectrum(std::string valueName, ValueType Owner::* const valuePtr) :
//		SdlValueType("spectrum", std::move(valueName), valuePtr)
//	{}
//
//	inline std::string valueAsString(const Spectrum& spectrum) const override
//	{
//		// TODO: add type, # values?
//		return spectrum.toString();
//	}
//
//protected:
//	inline void loadFromSdl(
//		Owner&                 owner,
//		const std::string&     sdlValue,
//		const SdlInputContext& ctx) const override
//	{
//		if(sdl::is_resource_identifier(sdlValue))
//		{
//			const SdlResourceIdentifier sdlResId(sdlValue, ctx.workingDirectory);
//
//			try
//			{
//				std::string loadedSdlValue = io_utils::load_text(sdlResId.getPathToResource());
//
//				setValue(
//					owner,
//					sdl::load_real_array(std::move(loadedSdlValue)));
//			}
//			catch(const FileIOError& e)
//			{
//				throw SdlLoadError("on loading real array -> " + e.whatStr());
//			}
//		}
//		else
//		{
//			setValue(owner, sdl::load_real_array(sdlValue));
//		}
//	}
//
//	void convertToSdl(
//		const Owner& owner,
//		std::string* out_sdlValue,
//		std::string& out_converterMessage) const override
//	{
//		PH_ASSERT(out_sdlValue);
//
//		// TODO
//		PH_ASSERT_UNREACHABLE_SECTION();
//	}
//};
//
//template<typename Owner, typename Element = real>
//using TSdlOptionalRealArray = TSdlRealArray<Owner, Element, TSdlOptionalValue<std::vector<Element>, Owner>>;

}// end namespace ph
