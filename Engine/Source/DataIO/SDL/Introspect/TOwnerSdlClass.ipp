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

		if(!isBlueprint())
		{
			return std::make_shared<Owner>();
		}
		else
		{
			return nullptr;
		}
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
	// Init base first just like how standard C++ does
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->initResource(resource, clauses, ctx);
	}

	Owner& ownerResource = castToOwnerType(resource);
	loadFieldsFromSdl(ownerResource, clauses, ctx);
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::initDefaultResource(ISdlResource& resource) const
{
	// Init base first just like how standard C++ does
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->initDefaultResource(resource);
	}

	Owner& ownerResource = castToOwnerType(resource);
	setFieldsToDefaults(ownerResource);
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::saveResource(
	const ISdlResource&     resource,
	OutputPayloads&         payloads,
	const SdlOutputContext& ctx) const
{
	// No specific ordering is required here. We save base class first just like how
	// the loading process is.
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->saveResource(resource, payloads, ctx);
	}

	const Owner& ownerResource = castToOwnerType(resource);
	saveFieldsToSdl(ownerResource, payloads, ctx);
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
			"cannot find SDL function named <" + std::string(funcName) + "> in "
			"SDL class <" + genPrettyName() + ">");
	}
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::associatedResources(
	const ISdlResource&               targetResource,
	std::vector<const ISdlResource*>& out_resources) const
{
	static_assert(std::is_base_of_v<ISdlResource, Owner>,
		"Owner class must derive from ISdlResource.");

	const Owner& ownerResource = castToOwnerType(targetResource);
	for(std::size_t fieldIdx = 0; fieldIdx < m_fields.numFields(); ++fieldIdx)
	{
		const TOwnedSdlField<Owner>& field = m_fields[fieldIdx];
		auto const associatedResource = field.associatedResource(ownerResource);
		if(associatedResource)
		{
			out_resources.push_back(associatedResource);
		}
	}

	// Find more associations in base class
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->associatedResources(targetResource, out_resources);
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
inline bool TOwnerSdlClass<Owner, FieldSet>::isBlueprint() const
{
	return std::is_abstract_v<Owner>;
}

template<typename Owner, typename FieldSet>
template<typename SdlFieldType>
inline auto TOwnerSdlClass<Owner, FieldSet>::addField(SdlFieldType sdlField)
	-> TOwnerSdlClass&
{
	// More restrictions on the type of SdlFieldType may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, SdlFieldType>,
		"Provided SdlFieldType is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename Owner, typename FieldSet>
template<typename StructType>
inline auto TOwnerSdlClass<Owner, FieldSet>::addStruct(StructType Owner::* const structObjPtr)
	-> TOwnerSdlClass&
{
	return addStruct(structObjPtr, SdlStructFieldStump());
}

template<typename Owner, typename FieldSet>
template<typename StructType>
inline auto TOwnerSdlClass<Owner, FieldSet>::addStruct(
	StructType Owner::* const  structObjPtr,
	const SdlStructFieldStump& structFieldStump)

	-> TOwnerSdlClass&
{
	// TODO: require StructType has getSdlFunction()

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
inline void TOwnerSdlClass<Owner, FieldSet>::loadFieldsFromSdl(
	Owner&                  owner, 
	ValueClauses&           clauses,
	const SdlInputContext&  ctx) const
{
	auto noticeReceiver = [](std::string noticeMsg, EFieldImportance importance)
	{
		if(importance == EFieldImportance::Optional || importance == EFieldImportance::NiceToHave)
		{
			PH_LOG_STRING(SdlClass, noticeMsg);
		}
		else
		{
			PH_LOG_WARNING_STRING(SdlClass, noticeMsg);
		}
	};

	// If this class is the source class, it should be the most derived class of the current
	// input process and consume all remaining clauses. Otherwise, redundant clauses are 
	// allowed as subsequent (derived) classes would consume them.
	//
	if(ctx.getSrcClass() == this)
	{
		field_set_op::load_fields_from_sdl(
			owner,
			m_fields,
			clauses,
			ctx,
			std::move(noticeReceiver));
	}
	else
	{
		field_set_op::load_fields_from_sdl_with_redundant_clauses(
			owner,
			m_fields,
			clauses,
			ctx,
			std::move(noticeReceiver));
	}
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::setFieldsToDefaults(Owner& owner) const
{
	for(std::size_t fieldIdx = 0; fieldIdx < m_fields.numFields(); ++fieldIdx)
	{
		const auto& field = m_fields[fieldIdx];

		// Set field to default value regardless of its importance (field importance is for import/export)
		field.setValueToDefault(owner);
	}
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::saveFieldsToSdl(
	const Owner&            owner,
	OutputPayloads&         payloads,
	const SdlOutputContext& ctx) const
{
	for(std::size_t fieldIdx = 0; fieldIdx < m_fields.numFields(); ++fieldIdx)
	{
		const TOwnedSdlField<Owner>& field = m_fields[fieldIdx];
		field.toSdl(owner, payloads.createPayload(), ctx);
	}
}

template<typename Owner, typename FieldSet>
inline auto TOwnerSdlClass<Owner, FieldSet>::description(std::string descriptionStr)
	-> TOwnerSdlClass&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename Owner, typename FieldSet>
inline auto TOwnerSdlClass<Owner, FieldSet>::docName(std::string docName)
	-> TOwnerSdlClass&
{
	setDocName(std::move(docName));
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
template<typename DeducedSrcType>
inline decltype(auto) TOwnerSdlClass<Owner, FieldSet>::castToOwnerType(DeducedSrcType& srcInstance) const
{
	// Source resource type, possibly cv-qualified
	using SrcType = std::remove_reference_t<DeducedSrcType>;

	static_assert(std::is_base_of_v<ISdlResource, SrcType>,
		"srcInstance must derive from ISdlResource.");

	static_assert(std::is_base_of_v<ISdlResource, Owner>,
		"Owner must derive from ISdlResource.");

	// Owner pointer type, possibly const-qualified (ignoring volatile)
	static_assert(!std::is_volatile_v<SrcType>);
	using OwnerPtr = std::conditional_t<std::is_const_v<SrcType>,
		const Owner*,
		Owner*>;

	OwnerPtr const ownerPtr = dynamic_cast<OwnerPtr>(&srcInstance);
	if(!ownerPtr)
	{
		throw_formatted<SdlException>(
			"type cast error: target resource is not owned by SDL class <{}> (resource ID = {})",
			genPrettyName(), srcInstance.getId());
	}

	// The parentheses are not required here--`*owner` is already an expression, resulting in a 
	// cv-qualified reference return type. Parentheses added just to be a reminder and also a fail-safe.
	return (*ownerPtr);
}

}// end namespace ph
