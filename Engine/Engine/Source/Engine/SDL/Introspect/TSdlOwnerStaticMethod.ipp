#pragma once

#include "Engine/SDL/Introspect/TSdlOwnerStaticMethod.h"
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

template<typename MethodStruct>
inline TSdlOwnerStaticMethod<MethodStruct>::TSdlOwnerStaticMethod() :
	SdlFunction()
{}

template<typename MethodStruct>
inline void TSdlOwnerStaticMethod<MethodStruct>::call(
	ISdlResource*          resource,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	PH_ASSERT(!resource);
	callStaticMethod(clauses, ctx);
}

template<typename MethodStruct>
inline void TSdlOwnerStaticMethod<MethodStruct>::callStaticMethod(
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	static_assert(!std::is_abstract_v<MethodStruct> && std::is_default_constructible_v<MethodStruct>,
		"MethodStruct must be non-abstract and default-constructible.");

	static_assert(std::is_invocable_v<MethodStruct>,
		"MethodStruct must contain an operator() that takes no argument.");

	MethodStruct methodStructObj{};
	loadParameters(
		methodStructObj,
		clauses,
		ctx);

	methodStructObj();
}

template<typename MethodStruct>
inline void TSdlOwnerStaticMethod<MethodStruct>::loadParameters(
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

template<typename MethodStruct>
inline std::size_t TSdlOwnerStaticMethod<MethodStruct>::numParams() const
{
	return m_fields.numFields();
}

template<typename MethodStruct>
inline const SdlField* TSdlOwnerStaticMethod<MethodStruct>::getParam(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename MethodStruct>
inline bool TSdlOwnerStaticMethod<MethodStruct>::isStatic() const
{
	return true;
}

template<typename MethodStruct>
template<typename T>
inline auto TSdlOwnerStaticMethod<MethodStruct>::addParam(T sdlField)
-> TSdlOwnerStaticMethod&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename MethodStruct>
inline auto TSdlOwnerStaticMethod<MethodStruct>::name(std::string nameStr)
-> TSdlOwnerStaticMethod&
{
	setName(std::move(nameStr));
	return *this;
}

template<typename MethodStruct>
inline auto TSdlOwnerStaticMethod<MethodStruct>::description(std::string descriptionStr)
-> TSdlOwnerStaticMethod&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename MethodStruct>
inline auto TSdlOwnerStaticMethod<MethodStruct>::userSpec(SdlUserSpec spec)
-> TSdlOwnerStaticMethod&
{
	setUserSpec(std::move(spec));
	return *this;
}

}// end namespace ph
