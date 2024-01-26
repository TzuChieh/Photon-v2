#pragma once

#include "SDL/Introspect/TSdlStructArray.ipp"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/ISdlDataPacketGroup.h"
#include "SDL/SdlNamedOutputClauses.h"
#include "SDL/Introspect/SdlStruct.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_helpers.h"
#include "SDL/sdl_traits.h"
#include "SDL/Tokenizer.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

template<typename Struct, typename Owner>
inline TSdlStructArray<Struct, Owner>::TSdlStructArray(
	std::string valueName,
	std::vector<Struct> Owner::* const valuePtr)

	: TSdlOwnedField<Owner>(
		"struct-array",
		std::move(valueName),
		"[S/" + getStructTypeName())

	, m_valuePtr(valuePtr)
{
	PH_ASSERT(m_valuePtr);
}

template<typename Struct, typename Owner>
inline void TSdlStructArray<Struct, Owner>::ownedValueToDefault(Owner& owner) const
{
	// Default value for an array is empty
	owner.*m_valuePtr = std::vector<Struct>{};
}

template<typename Struct, typename Owner>
inline std::string TSdlStructArray<Struct, Owner>::valueToString(const Owner& owner) const
{
	const std::vector<Struct>& structVector = owner.*m_valuePtr;

	return 
		"[" + std::to_string(structVector.size()) + " " +
		getStructTypeName() + " struct instances...]";
}

template<typename Struct, typename Owner>
inline void TSdlStructArray<Struct, Owner>::ownedResources(
	const Owner& owner,
	std::vector<const ISdlResource*>& out_resources) const
{
	if constexpr(!CHasSdlStructDefinition<Struct>)
	{
		return;
	}

	const std::vector<Struct>& storedVector = getValueVec(owner);
	for(const Struct& storedObj : storedVector)
	{
		Struct::getSdlStruct()->referencedResources(&storedObj, out_resources);
	}
}

template<typename Struct, typename Owner>
inline SdlNativeData TSdlStructArray<Struct, Owner>::ownedNativeData(Owner& owner) const
{
	std::vector<Struct>* const structVec = &(owner.*m_valuePtr);
	
	SdlNativeData data;
	if(structVec)
	{
		Struct* const structVecData = structVec->data();

		// Read-only for ordinary access to avoid accidental object slicing and other polymorphic
		// assignment issues. User should use direct accessor for assignment.
		data = SdlNativeData(
			[structVecData](std::size_t elementIdx) -> SdlGetterVariant
			{
				Struct* const structPtr = &structVecData[elementIdx];
				return structPtr
					? SdlNativeData::permissiveElementGetter(structPtr)
					: SdlConstInstance{};
			});

		data.numElements = structVec->size();
		data.setDirectAccessor(AnyNonConstPtr(structVec));
	}
	data.elementContainer = ESdlDataFormat::Vector;
	data.elementType = ESdlDataType::Struct;
	return data;
}

template<typename Struct, typename Owner>
inline void TSdlStructArray<Struct, Owner>::setValueVec(Owner& owner, std::vector<Struct> value) const
{
	owner.*m_valuePtr = std::move(value);
}

template<typename Struct, typename Owner>
inline const std::vector<Struct>& TSdlStructArray<Struct, Owner>::getValueVec(const Owner& owner) const
{
	return owner.*m_valuePtr;
}

template<typename Struct, typename Owner>
inline void TSdlStructArray<Struct, Owner>::loadFromSdl(
	Owner& owner,
	const SdlInputClause& clause,
	const SdlInputContext& ctx) const
{
	try
	{
		setValueVec(owner, loadStructArray(clause, ctx));
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlLoadError>(
			"unable to load resource on parsing struct array {} -> {}",
			valueToString(owner), e.whatStr());
	}
}

template<typename Struct, typename Owner>
inline void TSdlStructArray<Struct, Owner>::saveToSdl(
	const Owner& owner,
	SdlOutputClause& out_clause,
	const SdlOutputContext& ctx) const
{
	const std::vector<Struct>& structVector = getValueVec(owner);
	if(structVector.empty())
	{
		out_clause.isEmpty = true;
		return;
	}

	static_assert(CHasSdlStructDefinition<Struct>);

	// Basically generates a list of data packet names
	try
	{
		if(!ctx.getNamedOutputClauses())
		{
			throw SdlSaveError(
				"requires named output clauses to save");
		}

		out_clause.value = '{';
		for(const Struct& obj : structVector)
		{
			SdlOutputClauses objClauses;
			Struct::getSdlStruct()->saveObject(&obj, objClauses, ctx);

			const auto generatedName = ctx.getNamedOutputClauses()->addOrUpdate(objClauses);

			out_clause.value += generatedName;
			out_clause.value += ' ';
		}
		out_clause.value += '}';
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlSaveError>(
			"unable to save struct array {} -> {}", valueToString(owner), e.whatStr());
	}
}

template<typename Struct, typename Owner>
inline std::vector<Struct> TSdlStructArray<Struct, Owner>::loadStructArray(
	const SdlInputClause&  clause,
	const SdlInputContext& ctx)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'"', '"'}});

	try
	{
		std::vector<std::string> packetNameTokens;
		tokenizer.tokenize(clause.value, packetNameTokens);

		std::vector<Struct> structVector(packetNameTokens.size());
		for(std::size_t i = 0; i < structVector.size(); ++i)
		{
			structVector[i] = loadStruct(packetNameTokens[i], ctx);
		}

		return structVector;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing struct array -> " + e.whatStr());
	}
}

template<typename Struct, typename Owner>
inline Struct TSdlStructArray<Struct, Owner>::loadStruct(
	std::string_view packetName,
	const SdlInputContext& ctx)
{
	if(packetName.empty())
	{
		throw SdlLoadError(
			"packet name cannot be empty");
	}

	if(!ctx.getSrcDataPackets())
	{
		throw SdlLoadError(
			"no target data packet group specified");
	}

	const SdlInputClauses* const packet = ctx.getSrcDataPackets()->get(packetName);
	if(!packet)
	{
		throw_formatted<SdlLoadError>(
			"cannot find data packet with name <{}>", packetName);
	}

	static_assert(CHasSdlStructDefinition<Struct>);

	// OPT: remove this copy; currently init will modify packet (pop swap)
	SdlInputClauses copiedPacket = *packet;

	Struct obj{};
	Struct::getSdlStruct()->initObject(&obj, copiedPacket, ctx);
	return obj;
}

template<typename Struct, typename Owner>
inline std::string TSdlStructArray<Struct, Owner>::getStructTypeName()
{
	if constexpr(CHasSdlStructDefinition<Struct>)
	{
		PH_ASSERT(Struct::getSdlStruct());
		return std::string(Struct::getSdlStruct()->getTypeName());
	}
	else
	{
		return "struct";
	}
}

template<typename Struct, typename Owner>
inline auto TSdlStructArray<Struct, Owner>::withImportance(const EFieldImportance importance)
-> TSdlStructArray&
{
	this->setImportance(importance);
	return *this;
}

template<typename Struct, typename Owner>
inline auto TSdlStructArray<Struct, Owner>::description(std::string descriptionStr)
-> TSdlStructArray&
{
	this->setDescription(std::move(descriptionStr));
	return *this;
}

template<typename Struct, typename Owner>
inline auto TSdlStructArray<Struct, Owner>::optional()
-> TSdlStructArray&
{
	return withImportance(EFieldImportance::Optional);
}

template<typename Struct, typename Owner>
inline auto TSdlStructArray<Struct, Owner>::niceToHave()
-> TSdlStructArray&
{
	return withImportance(EFieldImportance::NiceToHave);
}

template<typename Struct, typename Owner>
inline auto TSdlStructArray<Struct, Owner>::required()
-> TSdlStructArray&
{
	return withImportance(EFieldImportance::Required);
}

}// end namespace ph
