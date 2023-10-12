#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlResourceLocator.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/sdl_exceptions.h"

#include <string>

namespace ph
{

/*! @brief A field class that binds a resource identifier member.
Though the member type that is binded by default can handle any types of identifier 
(the `ResourceIdentifier` class, which represents a PRI), this class treat it as a 
SDL Resource Identifier (SRI) which has a more specific form. On some circumstances 
it may be desirable to accept any identifier, and this can be achieved by specifying `IS_SRI=false`.
@tparam IS_SRI Whether the PRI is a SRI. If true, resolve happens during input and a failed resolve 
is an error. If false, the identifier is treated as a PRI and resolve is expected to be handled later
by the user.
*/
template<typename Owner, bool IS_SRI = true, typename SdlValueType = TSdlValue<ResourceIdentifier, Owner>>
class TSdlResourceIdentifier : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<ResourceIdentifier, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlResourceIdentifier(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("PRI", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const ResourceIdentifier& pri) const override
	{
		return pri.toString();
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		ResourceIdentifier* const identifier = this->getValue(owner);
		return SdlNativeData::fromSingleElement(
			identifier, ESdlDataFormat::Single, ESdlDataType::ResourceIdentifier, true, true);
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		if constexpr(IS_SRI)
		{
			SdlResourceLocator resolver(ctx);

			ResourceIdentifier sri(clause.value);
			if(sri.resolve(resolver))
			{
				this->setValue(owner, sri);
			}
			else
			{
				// Expected to be a valid SRI. It is an error if it is not a SRI (or cannot be resolved).
				throw_formatted<SdlLoadError>(
					"failed loading SRI -> cannot resolve {}", sri.getIdentifier());
			}
		}
		else
		{
			this->setValue(
				owner,
				ResourceIdentifier(clause.value));
		}
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		const ResourceIdentifier* const pri = this->getConstValue(owner);
		if(!pri)
		{
			return;
		}

		if constexpr(IS_SRI)
		{
			// Store the original identifier (the unresolved one)
			if(pri->hasIdentifier())
			{
				out_clause.value = pri->getIdentifier();
			}
			// Try to work out a SRI from path
			else
			{
				const Path& path = pri->getPath();
				if(path.isEmpty())
				{
					throw SdlSaveError(
						"failed saving SRI -> no valid information provided");
				}

				out_clause.value = SdlResourceLocator(ctx).toExternalIdentifier(path).getIdentifier();
			}
		}
		else
		{
			// Store the original identifier (the unresolved one)
			out_clause.value = pri->getIdentifier();
		}
	}
};

}// end namespace ph
