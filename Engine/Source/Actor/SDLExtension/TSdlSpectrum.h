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
#include "Core/Quantity/EQuantity.h"

#include <type_traits>
#include <string>
#include <vector>
#include <utility>

namespace ph
{

template<typename Owner, typename SdlValueType = TSdlValue<Spectrum, Owner>>
class TSdlSpectrum : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<Spectrum, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

public:
	template<typename ValueType>
	inline TSdlSpectrum(
		std::string valueName, 
		EQuantity usage,
		ValueType Owner::* const valuePtr) :

		SdlValueType("spectrum", std::move(valueName), valuePtr),

		m_usage(usage)
	{}

	inline std::string valueAsString(const Spectrum& spectrum) const override
	{
		// TODO: add type, # values?
		return spectrum.toString();
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlPayload&      payload,
		const SdlInputContext& ctx) const override
	{
		// TODO: allow for more color spaces via tag
		setValue(owner, sdl::load_spectrum(payload, m_usage));
	}

	void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override
	{
		PH_ASSERT(out_sdlValue);

		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}

private:
	EQuantity m_usage;
};

template<typename Owner>
using TSdlOptionalSpectrum = TSdlSpectrum<Owner, TSdlOptionalValue<Spectrum, Owner>>;

}// end namespace ph
