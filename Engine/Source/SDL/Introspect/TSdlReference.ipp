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
inline void TSdlReference<T, Owner>::setValueToDefault(Owner& owner) const
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
	constexpr ESdlDataType RES_TYPE = sdl::resource_type_of<T>();

	T* const originalDataPtr = (owner.*m_valuePtr).get();

	SdlNativeData data;
	
	// Cast to appropriate pointer type before assignment (casting to/from void* is only valid if the
	// exact same type is used)
	if constexpr(RES_TYPE == ESdlDataType::Geometry)
	{
		data = SdlNativeData(static_cast<Geometry*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::Material)
	{
		data = SdlNativeData(static_cast<Material*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::Motion)
	{
		data = SdlNativeData(static_cast<MotionSource*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::LightSource)
	{
		data = SdlNativeData(static_cast<LightSource*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::Actor)
	{
		data = SdlNativeData(static_cast<Actor*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::Image)
	{
		data = SdlNativeData(static_cast<Image*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::FrameProcessor)
	{
		data = SdlNativeData(static_cast<FrameProcessor*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::Observer)
	{
		data = SdlNativeData(static_cast<Observer*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::SampleSource)
	{
		data = SdlNativeData(static_cast<SampleSource*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::Visualizer)
	{
		data = SdlNativeData(static_cast<Visualizer*>(originalDataPtr));
	}
	else if constexpr(RES_TYPE == ESdlDataType::Option)
	{
		data = SdlNativeData(static_cast<Option*>(originalDataPtr));
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	data.format = ESdlDataFormat::Single;
	data.dataType = RES_TYPE;
	
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
	const SdlInputPayload& payload,
	const SdlInputContext& ctx) const
{
	try
	{
		setValueRef(owner, loadResource(payload, ctx));
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"unable to load resource on parsing reference " + 
			valueToString(owner) + " -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::saveToSdl(
	const Owner&            owner,
	SdlOutputPayload&       out_payload,
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

		sdl::save_field_id(this, out_payload);
		out_payload.value = resourceName;
	}
	catch(const SdlSaveError& e)
	{
		throw SdlSaveError(
			"unable to save resource reference " +
			valueToString(owner) + " -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
template<typename ResourceType>
inline std::shared_ptr<ResourceType> TSdlReference<T, Owner>::loadResource(
	const SdlInputPayload& payload,
	const SdlInputContext& ctx)
{
	const auto referenceName = payload.value;

	// TODO: get res should accept str view
	// TODO: allow type mismatch?

	PH_ASSERT(ctx.getRawScene());

	if(referenceName.empty() || referenceName.front() != '@')
	{
		throw SdlLoadError(
			"invalid reference name <" + payload.value + ">, should be prefixed with \'@\'");
	}

	auto resource = ctx.getRawScene()->getResources().get<ResourceType>(referenceName);
	if(!resource)
	{
		throw SdlLoadError(
			"cannot find resource referenced by <" + referenceName + ">");
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
