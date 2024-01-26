#pragma once

#include "SDL/Introspect/TSdlMethod.h"
#include "SDL/SdlInputClauses.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/Introspect/field_set_op.h"
#include "SDL/SceneDescription.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_traits.h"
#include "SDL/sdl_helpers.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

template<typename MethodStruct, typename TargetType>
inline TSdlMethod<MethodStruct, TargetType>::TSdlMethod(std::string name) :
	SdlFunction(std::move(name))
{}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::call(
	ISdlResource*          resource,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	static_assert(CHasSdlClassDefinition<TargetType>);

	if(!resource)
	{
		throw_formatted<SdlLoadError>(
			"cannot call SDL method without target resource ({})",
			sdl::gen_pretty_name(TargetType::getSdlClass()));
	}

	auto const targetRes = dynamic_cast<TargetType*>(resource);
	if(!targetRes)
	{
		throw_formatted<SdlLoadError>(
			"incompatible target resource, given {}, expected {}",
			sdl::gen_pretty_name(resource->getDynamicSdlClass()),
			sdl::gen_pretty_name(TargetType::getSdlClass()));
	}

	PH_ASSERT(targetRes);
	callMethod(*targetRes, clauses, ctx);
}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::callMethod(
	TargetType&            targetType, 
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	static_assert(!std::is_abstract_v<MethodStruct> && std::is_default_constructible_v<MethodStruct>,
		"MethodStruct must be non-abstract and default-constructible.");

	static_assert(std::is_invocable_v<MethodStruct, TargetType&>,
		"MethodStruct must contain an operator() that can take a TargetType instance.");

	MethodStruct methodStructObj{};
	loadParameters(
		methodStructObj,
		clauses,
		ctx);

	methodStructObj(targetType);
}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::loadParameters(
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
				PH_LOG_STRING(SdlFunction, noticeMsg);
			}
			else
			{
				PH_LOG_WARNING_STRING(SdlFunction, noticeMsg);
			}
		});
}

template<typename MethodStruct, typename TargetType>
inline std::size_t TSdlMethod<MethodStruct, TargetType>::numParams() const
{
	return m_fields.numFields();
}

template<typename MethodStruct, typename TargetType>
inline const SdlField* TSdlMethod<MethodStruct, TargetType>::getParam(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename MethodStruct, typename TargetType>
template<typename T>
inline auto TSdlMethod<MethodStruct, TargetType>::addParam(T sdlField)
	-> TSdlMethod&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename MethodStruct, typename TargetType>
inline auto TSdlMethod<MethodStruct, TargetType>::description(std::string descriptionStr)
	-> TSdlMethod&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

}// end namespace ph
