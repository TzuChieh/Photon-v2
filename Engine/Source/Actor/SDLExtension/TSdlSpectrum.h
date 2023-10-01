#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/Color/Spectrum.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlResourceLocator.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"
#include "Actor/SDLExtension/image_loaders.h"
#include "Actor/SDLExtension/sdl_color_enums.h"

#include <type_traits>
#include <string>
#include <vector>
#include <utility>

namespace ph
{

template<typename Owner, typename SdlValueType = TSdlValue<math::Spectrum, Owner>>
class TSdlSpectrum : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<math::Spectrum, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlSpectrum(
		std::string valueName, 
		const math::EColorUsage usage,
		ValueType Owner::* const valuePtr) :

		SdlValueType("spectrum", std::move(valueName), valuePtr),

		m_usage(usage)
	{}

	inline std::string valueAsString(const math::Spectrum& spectrum) const override
	{
		// TODO: add type, # values?
		return spectrum.toString();
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		math::Spectrum* const spectrum = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<math::Spectrum, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optSpectrum = this->valueRef(owner)](std::size_t /* elementIdx */) -> SdlGetterVariant
				{
					return optSpectrum
						? SdlNativeData::permissiveElementGetter(&(*optSpectrum))
						: std::monostate{};
				},
				[&optSpectrum = this->valueRef(owner)](std::size_t /* elementIdx */, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optSpectrum = std::nullopt;
						return true;
					}
					else
					{
						*optSpectrum = math::Spectrum{};
						return SdlNativeData::permissiveElementSetter(input, &(*optSpectrum));
					}
				},
				AnyNonConstPtr(spectrum));

			data.numElements = 1;
			data.elementContainer = ESdlDataFormat::Single;
			data.elementType = ESdlDataType::Spectrum;
			data.isNullClearable = true;
			return data;
		}
		else
		{
			return SdlNativeData::fromSingleElement(
				spectrum, ESdlDataFormat::Single, ESdlDataType::Spectrum, true, true);
		}
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		// TODO: allow for more color spaces via tag
		this->setValue(owner, sdl::load_spectrum(clause, m_usage));
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}

private:
	math::EColorUsage m_usage;
};

template<typename Owner>
using TSdlOptionalSpectrum = TSdlSpectrum<Owner, TSdlOptionalValue<math::Spectrum, Owner>>;

}// end namespace ph
