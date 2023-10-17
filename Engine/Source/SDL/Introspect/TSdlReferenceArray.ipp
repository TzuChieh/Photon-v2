#pragma once

#include "SDL/Introspect/TSdlReferenceArray.h"
#include "Common/assertion.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/ISdlResource.h"
#include "SDL/SceneDescription.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_helpers.h"
#include "Utility/string_utils.h"
#include "SDL/Tokenizer.h"

#include <utility>

namespace ph
{

class Geometry;
class Material;
class MotionSource;
class LightSource;
class Actor;
class Image;
class FrameProcessor;
class Observer;
class SampleSource;
class Visualizer;
class Option;

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
	owner.*m_valuePtr = std::vector<std::shared_ptr<T>>{};\
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
			"unable to load resource on parsing reference array {} -> {}",
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

	// Basically generates a list of reference names
	try
	{
		out_clause.value = '{';
		for(const std::shared_ptr<T>& resource : referenceVector)
		{
			const auto& resourceName = ctx.getDependencyResolver().getResourceName(resource.get());
			if(resourceName.empty())
			{
				throw SdlSaveError(
					"resource name is not tracked by the dependency resolver");
			}

			const bool hasWhitespaces = string_utils::has_any_of(resourceName, string_utils::get_whitespaces());
			out_clause.value += hasWhitespaces ? "\"" : "";
			out_clause.value += resourceName;
			out_clause.value += hasWhitespaces ? "\"" : "";
		}
		out_clause.value += '}';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError(
			"unable to save reference array " +
			valueToString(owner) + " -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
template<typename ResourceType>
inline std::vector<std::shared_ptr<T>> TSdlReferenceArray<T, Owner>::loadReferenceArray(
	const SdlInputClause& clause,
	const SdlInputContext& ctx)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'"', '"'}});

	try
	{
		std::vector<std::string> referenceTokens;
		tokenizer.tokenize(clause.value, referenceTokens);

		std::vector<std::shared_ptr<T>> referenceVector(referenceTokens.size());
		for(std::size_t i = 0; i < referenceVector.size(); ++i)
		{
			referenceVector[i] = loadReference(referenceTokens[i], ctx);
		}

		return referenceVector;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing reference array -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
template<typename ResourceType>
inline std::shared_ptr<ResourceType> TSdlReferenceArray<T, Owner>::loadReference(
	std::string_view referenceName,
	const SdlInputContext& ctx)
{
	if(referenceName.empty())
	{
		throw SdlLoadError(
			"reference name cannot be empty");
	}

	if(!ctx.getSrcReferences())
	{
		throw_formatted<SdlLoadError>(
			"no target reference group specified");
	}

	// TODO: allow type mismatch?
	// TODO: we may support some simple syntax such as wildcards or empty ref etc.
	auto resource = ctx.getSrcReferences()->getTyped<ResourceType>(referenceName);
	if(!resource)
	{
		throw_formatted<SdlLoadError>(
			"cannot find resource referenced by <{}>",
			referenceName);
	}

	return resource;
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
