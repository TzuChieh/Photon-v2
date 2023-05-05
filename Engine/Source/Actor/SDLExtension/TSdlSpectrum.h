#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/Color/Spectrum.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"
#include "Actor/SDLExtension/image_loaders.h"
#include "Actor/SDLExtension/sdl_color_usage_type.h"

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

		SdlNativeData data;
		if(spectrum)
		{
			data = SdlNativeData(spectrum);
		}

		data.format = ESdlDataFormat::Single;
		data.dataType = ESdlDataType::Spectrum;

		return data;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		// TODO: allow for more color spaces via tag
		this->setValue(owner, sdl::load_spectrum(payload, m_usage));
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
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
