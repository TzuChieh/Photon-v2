#pragma once

#include "Engine/SDL/Introspect/TSdlOwnerMethod.h"
#include "Engine/SDL/SdlInputClauses.h"
#include "Engine/SDL/Introspect/SdlInputContext.h"
#include "Engine/SDL/Introspect/field_set_op.h"
#include "Engine/SDL/SceneDescription.h"
#include "Engine/SDL/sdl_exceptions.h"
#include "Engine/SDL/sdl_traits.h"
#include "Engine/SDL/sdl_helpers.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

template<typename MethodStruct, typename Target>
inline TSdlOwnerMethod<MethodStruct, Target>::TSdlOwnerMethod() :
	SdlFunction()
{}

template<typename MethodStruct, typename Target>
inline void TSdlOwnerMethod<MethodStruct, Target>::call(
	ISdlResource*          resource,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	static_assert(CHasSdlClassDefinition<Target>);

	if(!resource)
	{
		throw_formatted<SdlLoadError>(
			"cannot call SDL method without target resource ({})",
			sdl::gen_pretty_name(Target::getSdlClass()));
	}

	auto const targetRes = dynamic_cast<Target*>(resource);
	if(!targetRes)
	{
		throw_formatted<SdlLoadError>(
			"incompatible target resource, given {}, expected {}",
			sdl::gen_pretty_name(resource->getDynamicSdlClass()),
			sdl::gen_pretty_name(Target::getSdlClass()));
	}

	PH_ASSERT(targetRes);
	callMethod(*targetRes, clauses, ctx);
}

template<typename MethodStruct, typename Target>
inline void TSdlOwnerMethod<MethodStruct, Target>::callMethod(
	Target&                target,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	static_assert(!std::is_abstract_v<MethodStruct> && std::is_default_constructible_v<MethodStruct>,
		"MethodStruct must be non-abstract and default-constructible.");

	static_assert(std::is_invocable_v<MethodStruct, Target&>,
		"MethodStruct must contain an operator() that can take a Target instance.");

	MethodStruct methodStructObj{};
	loadParameters(
		methodStructObj,
		clauses,
		ctx);

	methodStructObj(target);
}

template<typename MethodStruct, typename Target>
inline void TSdlOwnerMethod<MethodStruct, Target>::loadParameters(
	MethodStruct&          parameterStruct,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	field_set_op::load_fields_from_sdl(
		parameterStruct,
		m_fields,
		clauses,
		ctx,
		[](std::string noticeMsg, EFieldImportance importance)
		{
			if(importance == EFieldImportance::Optional || importance == EFieldImportance::NiceToHave)
			{
				PH_LOG_STRING(SdlFunction, Note, noticeMsg);
			}
			else
			{
				PH_LOG_STRING(SdlFunction, Warning, noticeMsg);
			}
		});
}

template<typename MethodStruct, typename Target>
inline std::size_t TSdlOwnerMethod<MethodStruct, Target>::numParams() const
{
	return m_fields.numFields();
}

template<typename MethodStruct, typename Target>
inline const SdlField* TSdlOwnerMethod<MethodStruct, Target>::getParam(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename MethodStruct, typename Target>
inline bool TSdlOwnerMethod<MethodStruct, Target>::isStatic() const
{
	return false;
}

template<typename MethodStruct, typename Target>
template<typename T>
inline auto TSdlOwnerMethod<MethodStruct, Target>::addParam(T sdlField)
-> TSdlOwnerMethod&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename MethodStruct, typename Target>
inline auto TSdlOwnerMethod<MethodStruct, Target>::name(std::string nameStr)
-> TSdlOwnerMethod&
{
	setName(std::move(nameStr));
	return *this;
}

template<typename MethodStruct, typename Target>
inline auto TSdlOwnerMethod<MethodStruct, Target>::description(std::string descriptionStr)
-> TSdlOwnerMethod&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename MethodStruct, typename Target>
inline auto TSdlOwnerMethod<MethodStruct, Target>::userSpec(SdlUserSpec spec)
-> TSdlOwnerMethod&
{
	setUserSpec(std::move(spec));
	return *this;
}

}// end namespace ph
