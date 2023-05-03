#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
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

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<std::vector<Element>, Owner>>
class TSdlRealArray : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<std::vector<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

public:
	template<typename ValueType>
	inline TSdlRealArray(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("real-array", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const std::vector<Element>& realArray) const override
	{
		return "[" + std::to_string(realArray.size()) + " real values...]";
	}

	inline ESdlDataFormat getNativeFormat() const override
	{
		return ESdlDataFormat::Vector;
	}

	inline ESdlDataType getNativeType() const override
	{
		return sdl::float_type_of<Element>();
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		if(payload.isResourceIdentifier())
		{
			const SdlResourceIdentifier sdlResId(payload.value, ctx.getWorkingDirectory());

			try
			{
				const std::string loadedSdlValue = io_utils::load_text(sdlResId.getPathToResource());
				this->setValue(owner, sdl::load_number_array<Element>(loadedSdlValue));
			}
			catch(const FileIOError& e)
			{
				throw SdlLoadError("on loading real array -> " + e.whatStr());
			}
		}
		else
		{
			this->setValue(owner, sdl::load_number_array<Element>(payload.value));
		}
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		// TODO: optionally as file

		if(const std::vector<Element>* const numberArr = this->getValue(owner); numberArr)
		{
			sdl::save_field_id(this, out_payload);
			sdl::save_number_array<Element>(*numberArr, &out_payload.value);
		}
	}
};

}// end namespace ph
