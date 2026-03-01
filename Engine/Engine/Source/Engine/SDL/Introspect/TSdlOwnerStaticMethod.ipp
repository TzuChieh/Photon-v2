#pragma once

#include "Engine/SDL/Introspect/TSdlOwnerStaticMethod.h"
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

template<typename MethodStruct, typename FieldSet>
inline TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::TSdlOwnerStaticMethod() :
	SdlFunction()
{}

template<typename MethodStruct, typename FieldSet>
inline void TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::call(
	ISdlResource*          resource,
	const SdlInstantiated* instantiated,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	PH_ASSERT(!resource);

	if(instantiated)
	{
		callStaticMethod(*instantiated, clauses, ctx);
	}
	else
	{
		if constexpr(std::is_default_constructible_v<MethodStruct>)
		{
			MethodStruct methodStructObj{};
			callStaticMethod(methodStructObj, clauses, ctx);
		}
		else
		{
			throw_formatted<SdlException>(
				"MethodStruct must be default-constructible for static method <{}>",
				genPrettyName());
		}
	}
}

template<typename MethodStruct, typename FieldSet>
inline void TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::callStaticMethod(
	MethodStruct&          functor,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	static_assert(std::is_invocable_v<MethodStruct>,
		"MethodStruct must contain an operator() that takes no argument.");

	loadParameters(
		functor,
		clauses,
		ctx);

	functor();
}

template<typename MethodStruct, typename FieldSet>
inline void TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::callStaticMethod(
	const SdlInstantiated& instantiated,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	MethodStruct* methodStructPtr = instantiated.data.get<MethodStruct>();
	if(!methodStructPtr)
	{
		throw_formatted<SdlException>(
			"invalid functor instance provided for static method <{}>", genPrettyName());
	}

	callStaticMethod(*methodStructPtr, clauses, ctx);
}

template<typename MethodStruct, typename FieldSet>
inline void TSdlOwnerStaticMethod<MethodStruct, FieldSet>
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

template<typename MethodStruct, typename FieldSet>
inline SdlInstantiated TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::instantiate() const
{
	auto allocation = std::make_shared<MethodStruct>();
	return {SdlNonConstInstance{allocation.get()}, allocation};
}

template<typename MethodStruct, typename FieldSet>
inline std::size_t TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::numParams() const
{
	return m_fields.numFields();
}

template<typename MethodStruct, typename FieldSet>
inline const SdlField* TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::getParam(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename MethodStruct, typename FieldSet>
inline bool TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::isStatic() const
{
	return true;
}

template<typename MethodStruct, typename FieldSet>
template<typename T>
inline auto TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::addParam(T sdlField)
-> TSdlOwnerStaticMethod&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename MethodStruct, typename FieldSet>
inline auto TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::name(std::string nameStr)
-> TSdlOwnerStaticMethod&
{
	setName(std::move(nameStr));
	return *this;
}

template<typename MethodStruct, typename FieldSet>
inline auto TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::description(std::string descriptionStr)
-> TSdlOwnerStaticMethod&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename MethodStruct, typename FieldSet>
inline auto TSdlOwnerStaticMethod<MethodStruct, FieldSet>
::userSpec(SdlUserSpec spec)
-> TSdlOwnerStaticMethod&
{
	setUserSpec(std::move(spec));
	return *this;
}

}// end namespace ph
