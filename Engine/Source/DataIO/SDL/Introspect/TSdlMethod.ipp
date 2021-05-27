#pragma once

#include "DataIO/SDL/Introspect/TSdlMethod.h"
#include "Common/assertion.h"
#include "DataIO/SDL/ValueClauses.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/Introspect/field_set_op.h"
#include "DataIO/SDL/NamedResourceStorage.h"
#include "DataIO/SDL/sdl_exceptions.h"

#include <utility>
#include <type_traits>

namespace ph
{

template<typename MethodStruct, typename TargetType>
inline TSdlMethod<MethodStruct, TargetType>::TSdlMethod(std::string name) :
	SdlFunction(std::move(name))
{}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::call(
	ISdlResource*          resource,
	ValueClauses&          clauses,
	const SdlInputContext& ctx) const
{
	if(!resource)
	{
		throw SdlLoadError(
			"cannot call SDL method without target resource "
			"(" + ctx.genPrettySrcClassName() + ")");
	}

	auto const targetRes = dynamic_cast<TargetType*>(resource);
	if(!targetRes)
	{
		throw SdlLoadError(
			"type cast error: target resource is not owned by "
			"SDL class <" + ctx.genPrettySrcClassName() + ">");
	}

	PH_ASSERT(targetRes);
	callMethod(*targetRes, clauses, ctx);
}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::callMethod(
	TargetType&            targetType, 
	ValueClauses&          clauses,
	const SdlInputContext& ctx) const
{
	MethodStruct methodStructObj;
	loadParameters(
		methodStructObj,
		clauses,
		ctx);

	methodStructObj(targetType);
}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::loadParameters(
	MethodStruct&          parameterStruct,
	ValueClauses&          clauses,
	const SdlInputContext& ctx) const
{
	field_set_op::load_fields_from_sdl(
		parameterStruct,
		m_fields,
		clauses,
		ctx,
		[](std::string noticeMsg, EFieldImportance importance)
		{
			if(importance == EFieldImportance::OPTIONAL || importance == EFieldImportance::NICE_TO_HAVE)
			{
				logger.log(ELogLevel::NOTE_MED, noticeMsg);
			}
			else
			{
				logger.log(ELogLevel::WARNING_MED, noticeMsg);
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

}// end namespace ph
