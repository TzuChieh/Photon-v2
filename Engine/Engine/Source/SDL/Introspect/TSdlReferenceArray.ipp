#pragma once

#include "SDL/Introspect/TSdlReferenceArray.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/Introspect/TSdlReference.h"
#include "SDL/SdlDependencyResolver.h"
#include "SDL/ISdlResource.h"
#include "SDL/SceneDescription.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_helpers.h"
#include "SDL/sdl_parser.h"
#include "SDL/Tokenizer.h"

#include <Common/assertion.h>
#include <Utility/string_utils.h>

#include <utility>

namespace ph
{

template<typename T, typename Owner>
inline TSdlReferenceArray<T, Owner>::TSdlReferenceArray(
	std::string valueName,
	std::vector<std::shared_ptr<T>> Owner::* const valuePtr)

	: TSdlOwnedField<Owner>(
		std::string(sdl::category_to_string(sdl::category_of<T>())) + "-array",
		std::move(valueName))

	, m_valuePtr(valuePtr)
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T must be a SDL resource (derive from ISdlResource).");

	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlReferenceArray<T, Owner>::ownedValueToDefault(Owner& owner) const
{
	// Default value for an array is empty
	owner.*m_valuePtr = std::vector<std::shared_ptr<T>>{};
}

template<typename T, typename Owner>
inline std::string TSdlReferenceArray<T, Owner>::valueToString(const Owner& owner) const
{
	const std::vector<std::shared_ptr<T>>& referenceVector = owner.*m_valuePtr;

	return 
		"[" + std::to_string(referenceVector.size()) + " " +
		std::string(sdl::category_to_string(sdl::category_of<T>())) + " references...]";
}

template<typename T, typename Owner>
inline void TSdlReferenceArray<T, Owner>::ownedResources(
	const Owner& owner,
	std::vector<const ISdlResource*>& out_resources) const
{
	const std::vector<std::shared_ptr<T>>& storedVector = getValueVec(owner);
	for(const std::shared_ptr<T>& storedResource : storedVector)
	{
		out_resources.push_back(storedResource.get());
	}
}

template<typename T, typename Owner>
inline SdlNativeData TSdlReferenceArray<T, Owner>::ownedNativeData(Owner& owner) const
{
	std::vector<std::shared_ptr<T>>* const refVec = &(owner.*m_valuePtr);
	
	SdlNativeData data;
	if(refVec)
	{
		std::shared_ptr<T>* const refVecData = refVec->data();

		// Read-only for ordinary access to avoid accidental object slicing and other polymorphic
		// assignment issues. User should use direct accessor for assignment.
		data = SdlNativeData(
			[refVecData](std::size_t elementIdx) -> SdlGetterVariant
			{
				T* const originalDataPtr = refVecData[elementIdx].get();
				return originalDataPtr
					? SdlNativeData::permissiveElementGetter(originalDataPtr)
					: SdlConstInstance{};
			});

		data.numElements = refVec->size();
		data.setDirectAccessor(AnyNonConstPtr(refVec));
	}
	data.elementContainer = ESdlDataFormat::SharedPtrVector;
	data.elementType = sdl::resource_type_of<T>();
	return data;
}

template<typename T, typename Owner>
inline void TSdlReferenceArray<T, Owner>::setValueVec(
	Owner& owner, 
	std::vector<std::shared_ptr<T>> value) const
{
	owner.*m_valuePtr = std::move(value);
}

template<typename T, typename Owner>
inline auto TSdlReferenceArray<T, Owner>::getValueVec(const Owner& owner) const
-> const std::vector<std::shared_ptr<T>>&
{
	return owner.*m_valuePtr;
}

template<typename T, typename Owner>
inline void TSdlReferenceArray<T, Owner>::loadFromSdl(
	Owner& owner,
	const SdlInputClause& clause,
	const SdlInputContext& ctx) const
{
	try
	{
		setValueVec(owner, loadReferenceArray(clause, ctx));
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlLoadError>(
			"on parsing reference array {} -> {}",
			valueToString(owner), e.whatStr());
	}
}

template<typename T, typename Owner>
inline void TSdlReferenceArray<T, Owner>::saveToSdl(
	const Owner& owner,
	SdlOutputClause& out_clause,
	const SdlOutputContext& ctx) const
{
	const std::vector<std::shared_ptr<T>>& referenceVector = getValueVec(owner);
	if(referenceVector.empty())
	{
		out_clause.isEmpty = true;
		return;
	}

	// Basically generates a list of reference names  (with proper specifier)
	try
	{
		out_clause.value = '{';
		for(const std::shared_ptr<T>& resource : referenceVector)
		{
			const auto resourceName = ctx.getResourceName(resource.get());
			if(resourceName.empty())
			{
				throw SdlSaveError(
					"resource name unavailable");
			}

			const bool hasWhitespaces = string_utils::has_any_of(resourceName, string_utils::get_whitespaces());
			out_clause.value += sdl_parser::persistent_specifier;
			out_clause.value += hasWhitespaces ? "\"" : "";
			out_clause.value += resourceName;
			out_clause.value += hasWhitespaces ? "\"" : "";
			out_clause.value += ' ';
		}
		out_clause.value += '}';
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlSaveError>(
			"on saving reference array {} -> {}", valueToString(owner), e.whatStr());
	}
}

template<typename T, typename Owner>
template<typename ResourceType>
inline std::vector<std::shared_ptr<T>> TSdlReferenceArray<T, Owner>::loadReferenceArray(
	const SdlInputClause& clause,
	const SdlInputContext& ctx)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'"', '"'}});

	if(!ctx.getSrcResources())
	{
		throw SdlLoadError(
			"no target reference group specified");
	}

	if(clause.valueType == ESdlClauseValue::PersistentTargetName)
	{
		auto resource = TSdlReference<T, Owner>::loadReference(clause, ctx);
		return {resource};
	}
	else if(clause.valueType == ESdlClauseValue::General)
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(clause.value, tokens);
		if(tokens.size() % 2 != 0)
		{
			throw SdlLoadError(
				"syntax error: unexpected input format");
		}

		const auto numReferenceTokens = tokens.size() / 2;
		std::vector<std::shared_ptr<T>> referenceVector(numReferenceTokens);
		for(std::size_t i = 0; i < numReferenceTokens; ++i)
		{
			const std::string referenceToken = tokens[i * 2] + tokens[i * 2 + 1];
			const auto reference = sdl_parser::get_reference(referenceToken);
			referenceVector[i] = TSdlReference<T, Owner>::loadReference(reference, ctx);
		}

		return referenceVector;
	}
	else
	{
		throw SdlLoadError(
			"bad reference type (only persistent target is supported)");
	}
}

template<typename T, typename Owner>
inline auto TSdlReferenceArray<T, Owner>::withImportance(const EFieldImportance importance)
-> TSdlReferenceArray&
{
	this->setImportance(importance);
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlReferenceArray<T, Owner>::description(std::string descriptionStr)
-> TSdlReferenceArray&
{
	this->setDescription(std::move(descriptionStr));
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlReferenceArray<T, Owner>::optional()
-> TSdlReferenceArray&
{
	return withImportance(EFieldImportance::Optional);
}

template<typename T, typename Owner>
inline auto TSdlReferenceArray<T, Owner>::niceToHave()
-> TSdlReferenceArray&
{
	return withImportance(EFieldImportance::NiceToHave);
}

template<typename T, typename Owner>
inline auto TSdlReferenceArray<T, Owner>::required()
-> TSdlReferenceArray&
{
	return withImportance(EFieldImportance::Required);
}

}// end namespace ph
