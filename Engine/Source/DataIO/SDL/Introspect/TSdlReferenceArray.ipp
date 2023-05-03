#pragma once

#include "DataIO/SDL/Introspect/TSdlReferenceArray.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/Tokenizer.h"

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

	: TOwnedSdlField<Owner>(
		sdl::category_to_string(sdl::category_of<T>()) + "-array",
		std::move(valueName))

	, m_valuePtr(valuePtr)
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T must be a SDL resource (derive from ISdlResource).");

	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlReferenceArray<T, Owner>::setValueToDefault(Owner& owner) const
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
		sdl::category_to_string(sdl::category_of<T>()) + " references...]";
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
	std::vector<std::shared_ptr<T>>* const referenceVectorPtr = &(owner.*m_valuePtr);

	SdlNativeData data;
	data.format = ESdlDataFormat::SharedPtrVector;

	// C++ does not support covariant array/vector, so only a couple of cases (where `T` is an exact
	// match to one of the base resource type) are fully supported. For other cases, data pointer is
	// still provided though the user need to figure out the actual type themselves.
	if constexpr(
		CSame<T, Geometry> ||
		CSame<T, Material> ||
		CSame<T, MotionSource> || 
		CSame<T, LightSource> || 
		CSame<T, Actor> || 
		CSame<T, Image> || 
		CSame<T, FrameProcessor> || 
		CSame<T, Observer> || 
		CSame<T, SampleSource> || 
		CSame<T, Visualizer> || 
		CSame<T, Option>)
	{
		data.dataType = sdl::resource_type_of<T>();
	}
	else
	{
		data.dataType = ESdlDataType::None;
	}
	data.dataPtr = &(owner.*m_valuePtr);
	
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
	const SdlInputPayload& payload,
	const SdlInputContext& ctx) const
{
	try
	{
		setValueVec(owner, loadReferenceArray(payload, ctx));
	}
	catch(const SdlLoadError& e)
	{
		throw_formatted<SdlLoadError>(
			"unable to load resource on parsing reference array {} -> {}",
			valueToString(owner), e.whatStr());
	}
}

template<typename T, typename Owner>
inline void TSdlReferenceArray<T, Owner>::saveToSdl(
	const Owner& owner,
	SdlOutputPayload& out_payload,
	const SdlOutputContext& ctx) const
{
	const std::vector<std::shared_ptr<T>>& referenceVector = getValueVec(owner);

	try
	{
		sdl::save_field_id(this, out_payload);

		out_payload.value = '{';
		for(const std::shared_ptr<T>& resource : referenceVector)
		{
			const auto& resourceName = ctx.getReferenceResolver().getResourceName(resource.get());
			if(resourceName.empty())
			{
				throw SdlSaveError(
					"resource name is not tracked by the reference resolver");
			}

			out_payload.value += "\"";
			out_payload.value += resourceName;
			out_payload.value += "\"";
		}
		out_payload.value += '}';
	}
	catch(const SdlSaveError& e)
	{
		throw SdlSaveError(
			"unable to save reference array " +
			valueToString(owner) + " -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
template<typename ResourceType>
inline std::vector<std::shared_ptr<T>> TSdlReferenceArray<T, Owner>::loadReferenceArray(
	const SdlInputPayload& payload,
	const SdlInputContext& ctx)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

	try
	{
		std::vector<std::string> referenceTokens;
		tokenizer.tokenize(payload.value, referenceTokens);

		std::vector<std::shared_ptr<T>> referenceVector(referenceTokens.size());
		for(std::size_t i = 0; i < referenceVector.size(); ++i)
		{
			referenceVector[i] = loadReference(referenceTokens[i], ctx);
		}

		return referenceVector;
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing reference array -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
template<typename ResourceType>
inline std::shared_ptr<ResourceType> TSdlReferenceArray<T, Owner>::loadReference(
	const std::string_view referenceName,
	const SdlInputContext& ctx)
{
	// TODO: get res should accept str view
	// TODO: allow type mismatch?
	// TODO: we may support some simple syntax such as wildcards or empty ref etc.

	PH_ASSERT(ctx.getRawScene());

	if(referenceName.empty() || referenceName.front() != '@')
	{
		throw_formatted<SdlLoadError>(
			"invalid reference name <{}>, should be prefixed with \'@\'",
			referenceName);
	}

	auto resource = ctx.getRawScene()->getResources().get<ResourceType>(std::string(referenceName));
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
