#pragma once

#include "DataIO/SDL/Introspect/TOwnerSdlClass.h"
#include "DataIO/SDL/Introspect/field_set_op.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/SdlStructFieldStump.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <type_traits>

namespace ph
{

template<typename Owner, typename FieldSet>
inline TOwnerSdlClass<Owner, FieldSet>::TOwnerSdlClass(std::string displayName) :

	SdlClass(sdl::category_of<Owner>(), std::move(displayName)),

	m_fields(),
	m_functions()
{}

template<typename Owner, typename FieldSet>
inline std::shared_ptr<ISdlResource> TOwnerSdlClass<Owner, FieldSet>::createResource() const
{
	if constexpr(!std::is_abstract_v<Owner>)
	{
		static_assert(std::is_default_constructible_v<Owner>,
			"A non-abstract owner class must have a default constructor.");

		return std::make_shared<Owner>();
	}
	else
	{
		return nullptr;
	}
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::initResource(
	ISdlResource&          resource,
	ValueClauses&          clauses,
	const SdlInputContext& ctx) const
{
	static_assert(std::is_base_of_v<ISdlResource, Owner>,
		"Owner class must derive from ISdlResource.");

	// Init base first just like C++ does
	if(isDerived())
	{
		getBase()->initResource(resource, clauses, ctx);
	}

	Owner* const owner = dynamic_cast<Owner*>(&resource);
	if(!owner)
	{
		throw SdlLoadError(
			"type cast error: target resource is not owned by "
			"SDL class <" + genPrettyName() + ">");
	}

	PH_ASSERT(owner);
	fromSdl(*owner, clauses, ctx);
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::call(
	const std::string_view funcName,
	ISdlResource* const    resource,
	ValueClauses&          clauses,
	const SdlInputContext& ctx) const
{
	// Find SDL function by name
	const SdlFunction* func = nullptr;
	for(std::size_t funcIdx = 0; funcIdx < m_functions.size(); ++funcIdx)
	{
		PH_ASSERT(m_functions[funcIdx]);
		const SdlFunction& funcI = *(m_functions[funcIdx]);
		if(funcI.getName() == funcName)
		{
			func = &funcI;
			break;
		}
	}

	// If found the function in this class, call it
	if(func)
	{
		func->call(resource, clauses, ctx);
	}
	// If not found in this class, call on base class instead
	else if(getBase())
	{
		getBase()->call(funcName, resource, clauses, ctx);
	}
	else
	{
		throw SdlLoadError(
			"cannot find SDL function named <" + std::string(funcName) + "> in"
			"SDL class <" + genPrettyName() + ">");
	}
}

template<typename Owner, typename FieldSet>
inline std::size_t TOwnerSdlClass<Owner, FieldSet>::numFields() const
{
	return m_fields.numFields();
}

template<typename Owner, typename FieldSet>
inline const SdlField* TOwnerSdlClass<Owner, FieldSet>::getField(const std::size_t index) const
{
	return getOwnedField(index);
}

template<typename Owner, typename FieldSet>
inline std::size_t TOwnerSdlClass<Owner, FieldSet>::numFunctions() const
{
	return m_functions.size();
}

template<typename Owner, typename FieldSet>
inline const SdlFunction* TOwnerSdlClass<Owner, FieldSet>::getFunction(const std::size_t index) const
{
	return index < m_functions.size() ? m_functions[index] : nullptr;
}

template<typename Owner, typename FieldSet>
inline const TOwnedSdlField<Owner>* TOwnerSdlClass<Owner, FieldSet>::getOwnedField(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename Owner, typename FieldSet>
template<typename T>
inline auto TOwnerSdlClass<Owner, FieldSet>::addField(T sdlField)
	-> TOwnerSdlClass&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename Owner, typename FieldSet>
template<typename T>
inline auto TOwnerSdlClass<Owner, FieldSet>::addStruct(T Owner::* const structObjPtr)
	-> TOwnerSdlClass&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlStruct, T>,
		"T is not a SdlStruct thus cannot be added.");

	PH_ASSERT(structObjPtr);

	m_fields.addFields(SdlStructFieldStump().genFieldSet(structObjPtr));

	return *this;
}

template<typename Owner, typename FieldSet>
template<typename T>
inline auto TOwnerSdlClass<Owner, FieldSet>::addStruct(
	T Owner::* const           structObjPtr,
	const SdlStructFieldStump& structFieldStump)

	-> TOwnerSdlClass&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlStruct, T>,
		"T is not a SdlStruct thus cannot be added.");

	PH_ASSERT(structObjPtr);

	m_fields.addFields(structFieldStump.genFieldSet(structObjPtr));

	return *this;
}

template<typename Owner, typename FieldSet>
template<typename T>
inline auto TOwnerSdlClass<Owner, FieldSet>::addFunction()
	-> TOwnerSdlClass&
{
	// TODO: require T has getSdlFunction()

	m_functions.pushBack(T::getSdlFunction());

	return *this;
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::fromSdl(
	Owner&                  owner, 
	ValueClauses&           clauses,
	const SdlInputContext&  ctx) const
{
	field_set_op::load_fields_from_sdl(
		owner,
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

template<typename Owner, typename FieldSet>
inline auto TOwnerSdlClass<Owner, FieldSet>::description(std::string descriptionStr)
	-> TOwnerSdlClass&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename Owner, typename FieldSet>
template<typename T>
inline auto TOwnerSdlClass<Owner, FieldSet>::baseOn()
	-> TOwnerSdlClass&
{
	static_assert(!std::is_same_v<T, Owner>,
		"A SDL class cannot base on itself.");

	setBase<T>();
	return *this;
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::toSdl(
	const Owner&       owner,
	std::string* const out_sdl,
	std::string&       out_message) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
