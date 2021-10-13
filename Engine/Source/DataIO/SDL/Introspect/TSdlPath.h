#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/sdl_exceptions.h"

#include <string>

namespace ph
{

template<typename Owner, typename SdlValueType = TSdlValue<Path, Owner>>
class TSdlPath : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<Path, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

public:
	template<typename ValueType>
	inline TSdlPath(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("path", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const std::string& str) const override
	{
		return str;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		if(payload.isResourceIdentifier())
		{
			const SdlResourceIdentifier resId(payload.value, ctx.getWorkingDirectory());
			this->setValue(owner, resId.getPathToResource());
		}
		else
		{
			throw SdlLoadError("input value is not a SDL resource identifier");
		}
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		if(const Path* const path = this->getValue(owner); path)
		{
			// TODO
		}
	}
};

template<typename Owner>
using TSdlOptionalPath = TSdlPath<Owner, TSdlOptionalValue<Path, Owner>>;

}// end namespace ph
