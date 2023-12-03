#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"

#include <string>

namespace ph
{

template<typename Owner, typename SdlValueType = TSdlValue<Path, Owner>>
class TSdlPath : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<Path, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlPath(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("path", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const Path& path) const override
	{
		return path.toString();
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		Path* const path = this->getValue(owner);
		return SdlNativeData::fromSingleElement(
			path, ESdlDataFormat::Single, ESdlDataType::Path, true, true);
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, Path(clause.value));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		// Paths are always enclosed in double quotes, they have high chance to contain whitespaces
		out_clause.value = '"';

		if(const Path* path = this->getConstValue(owner); path)
		{
			out_clause.value += path->toString();
		}

		out_clause.value += '"';
	}
};

}// end namespace ph