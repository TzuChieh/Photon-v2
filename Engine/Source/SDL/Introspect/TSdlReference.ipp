#pragma once

#include "SDL/Introspect/TSdlReference.h"
#include "Common/assertion.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/ISdlResource.h"
#include "SDL/SceneDescription.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_helpers.h"
#include "Utility/string_utils.h"

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
inline TSdlReference<T, Owner>::TSdlReference(
	std::string                       valueName,
	std::shared_ptr<T> Owner::* const valuePtr) :

	TSdlOwnedField<Owner>(
		std::string(sdl::category_to_string(sdl::category_of<T>())),
		std::move(valueName)),

	m_valuePtr(valuePtr)
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T must be a SDL resource (derive from ISdlResource).");

	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::ownedValueToDefault(Owner& owner) const
{
	// Default value for a SDL resource defined as nullptr
	setValueRef(owner, nullptr);
}

template<typename T, typename Owner>
inline std::string TSdlReference<T, Owner>::valueToString(const Owner& owner) const
{
	return 
		"[" + std::string(sdl::category_to_string(sdl::category_of<T>())) + " ref: " +
		std::string(getValueRef(owner) ? "valid" : "empty") + "]";
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::ownedResources(
	const Owner& owner,
	std::vector<const ISdlResource*>& out_resources) const
{
	const T* const storedResource = getValueRef(owner).get();
	if(storedResource != nullptr)
	{
		out_resources.push_back(storedResource);
	}
}

template<typename T, typename Owner>
inline SdlNativeData TSdlReference<T, Owner>::ownedNativeData(Owner& owner) const
{
	std::shared_ptr<T>& valueRef = owner.*m_valuePtr;
	T* const originalDataPtr = valueRef.get();

	// Read-only for ordinary access to avoid accidental object slicing and other polymorphic
	// assignment issues. User should use direct accessor for assignment.
	SdlNativeData data = SdlNativeData(
		[originalDataPtr](std::size_t elementIdx) -> SdlGetterVariant
		{
			return originalDataPtr
				? SdlNativeData::permissiveElementGetter(originalDataPtr)
				: SdlConstInstance{};
		});
	data.setDirectAccessor(AnyNonConstPtr(&valueRef));
	data.elementContainer = ESdlDataFormat::SharedPointer;
	data.elementType = sdl::resource_type_of<T>();
	return data;
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::setValueRef(Owner& owner, std::shared_ptr<T> value) const
{
	owner.*m_valuePtr = std::move(value);
}

template<typename T, typename Owner>
inline const std::shared_ptr<T>& TSdlReference<T, Owner>::getValueRef(const Owner& owner) const
{
	return owner.*m_valuePtr;
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::loadFromSdl(
	Owner&                 owner,
	const SdlInputClause&  clause,
	const SdlInputContext& ctx) const
{
	try
	{
		setValueRef(owner, loadResource(clause, ctx));
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError(
			"unable to load resource on parsing reference " + 
			valueToString(owner) + " -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::saveToSdl(
	const Owner&            owner,
	SdlOutputClause&        out_clause,
	const SdlOutputContext& ctx) const
{
	const auto& resource = getValueRef(owner);
	if(!resource)
	{
		return;
	}

	try
	{
		const auto& resourceName = ctx.getDependencyResolver().getResourceName(resource.get());
		if(resourceName.empty())
		{
			throw SdlSaveError(
				"resource name is not tracked by the reference resolver");
		}

		out_clause.value = resourceName;
		out_clause.isReference = true;
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError(
			"unable to save resource reference " +
			valueToString(owner) + " -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
template<typename ResourceType>
inline std::shared_ptr<ResourceType> TSdlReference<T, Owner>::loadResource(
	const SdlInputClause& clause,
	const SdlInputContext& ctx)
{
	const auto& referenceName = clause.value;
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
inline auto TSdlReference<T, Owner>::withImportance(const EFieldImportance importance)
-> TSdlReference&
{
	this->setImportance(importance);
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::description(std::string descriptionStr)
-> TSdlReference&
{
	this->setDescription(std::move(descriptionStr));
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::optional()
-> TSdlReference&
{
	return withImportance(EFieldImportance::Optional);
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::niceToHave()
-> TSdlReference&
{
	return withImportance(EFieldImportance::NiceToHave);
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::required()
-> TSdlReference&
{
	return withImportance(EFieldImportance::Required);
}

}// end namespace ph
