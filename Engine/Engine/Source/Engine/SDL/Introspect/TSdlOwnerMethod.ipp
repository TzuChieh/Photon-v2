#pragma once

#include "Engine/SDL/Introspect/TSdlOwnerMethod.h"
#include "Engine/SDL/SdlInputClauses.h"
#include "Engine/SDL/Introspect/SdlInstantiated.h"
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

template<typename MethodStruct, typename Target, typename FieldSet>
inline TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::TSdlOwnerMethod() :
	SdlFunction()
{}

template<typename MethodStruct, typename Target, typename FieldSet>
inline void TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::call(
	ISdlResource*          resource,
	const SdlInstantiated* instantiated,
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

	if(instantiated)
	{
		callMethod(*targetRes, *instantiated, clauses, ctx);
	}
	else
	{
		if constexpr(std::is_default_constructible_v<MethodStruct>)
		{
			MethodStruct methodStructObj{};
			callMethod(*targetRes, methodStructObj, clauses, ctx);
		}
		else
		{
			throw_formatted<SdlException>(
				"MethodStruct must be default-constructible for method <{}>",
				genPrettyName());
		}
	}
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline void TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::callMethod(
	Target&                target,
	MethodStruct&          functor,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	static_assert(std::is_invocable_v<MethodStruct, Target&>,
		"MethodStruct must contain an operator() that can take a Target instance.");

	loadParameters(
		functor,
		clauses,
		ctx);

	functor(target);
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline void TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::callMethod(
	Target&                target,
	const SdlInstantiated& instantiated,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	MethodStruct* methodStructPtr = instantiated.data.get<MethodStruct>();
	if(!methodStructPtr)
	{
		throw_formatted<SdlException>(
			"invalid functor instance provided for method <{}>", genPrettyName());
	}

	callMethod(target, *methodStructPtr, clauses, ctx);
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline void TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::loadParameters(
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

template<typename MethodStruct, typename Target, typename FieldSet>
inline SdlInstantiated TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::instantiate() const
{
	auto allocation = std::make_shared<MethodStruct>();
	return {SdlNonConstInstance{allocation.get()}, allocation};
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline std::size_t TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::numParams() const
{
	return m_fields.numFields();
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline const SdlField* TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::getParam(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline bool TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::isStatic() const
{
	return false;
}

template<typename MethodStruct, typename Target, typename FieldSet>
template<typename T>
inline auto TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::addParam(T sdlField)
-> TSdlOwnerMethod&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline auto TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::name(std::string nameStr)
-> TSdlOwnerMethod&
{
	setName(std::move(nameStr));
	return *this;
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline auto TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::description(std::string descriptionStr)
-> TSdlOwnerMethod&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename MethodStruct, typename Target, typename FieldSet>
inline auto TSdlOwnerMethod<MethodStruct, Target, FieldSet>
::userSpec(SdlUserSpec spec)
-> TSdlOwnerMethod&
{
	setUserSpec(std::move(spec));
	return *this;
}

}// end namespace ph
