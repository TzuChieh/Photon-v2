#pragma once

#include "SDL/Introspect/TSdlOwnerClass.h"
#include "SDL/Introspect/field_set_op.h"
#include "SDL/Introspect/SdlStruct.h"
#include "SDL/Introspect/SdlStructFieldStump.h"
#include "Common/assertion.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/Introspect/SdlFunction.h"
#include "SDL/sdl_helpers.h"

#include <type_traits>
#include <format>

namespace ph
{

template<typename Owner, typename FieldSet>
inline TSdlOwnerClass<Owner, FieldSet>::TSdlOwnerClass(std::string displayName)

	: SdlClass(sdl::category_of<Owner>(), std::move(displayName))

	, m_fields()
	, m_functions()
{
	setIsBlueprint(std::is_abstract_v<Owner>);
}

template<typename Owner, typename FieldSet>
inline std::shared_ptr<ISdlResource> TSdlOwnerClass<Owner, FieldSet>::createResource() const
{
	if constexpr(!std::is_abstract_v<Owner> && std::is_default_constructible_v<Owner>)
	{
		if(!isBlueprint() && allowCreateFromClass())
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
		// The resource is abstract and/or not default-constructible, make sure this is intended
		PH_ASSERT_MSG(isBlueprint() || !allowCreateFromClass(),
			std::format("Cannot create resource while the definition permits it (is blueprint: {}, "
			"allow create from class: {})", isBlueprint(), allowCreateFromClass()));

		return nullptr;
	}
}

template<typename Owner, typename FieldSet>
inline void TSdlOwnerClass<Owner, FieldSet>::initResource(
	ISdlResource&          resource,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	// Init base first just like how standard C++ does
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->initResource(resource, clauses, ctx);
	}

	Owner* const ownerResource = castTo<Owner>(&resource);
	loadFieldsFromSdl(*ownerResource, clauses, ctx);
}

template<typename Owner, typename FieldSet>
inline void TSdlOwnerClass<Owner, FieldSet>::initDefaultResource(ISdlResource& resource) const
{
	// Init base first just like how standard C++ does
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->initDefaultResource(resource);
	}

	Owner* const ownerResource = castTo<Owner>(&resource);
	setFieldsToDefaults(*ownerResource);
}

template<typename Owner, typename FieldSet>
inline void TSdlOwnerClass<Owner, FieldSet>::saveResource(
	const ISdlResource&     resource,
	SdlOutputClauses&       out_clauses,
	const SdlOutputContext& ctx) const
{
	// No specific ordering is required here. We save base class first just like how
	// the loading process is.
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->saveResource(resource, out_clauses, ctx);
	}

	const Owner* const ownerResource = castTo<const Owner>(&resource);
	saveFieldsToSdl(*ownerResource, out_clauses, ctx);
}

template<typename Owner, typename FieldSet>
inline void TSdlOwnerClass<Owner, FieldSet>::call(
	const std::string_view funcName,
	ISdlResource* const    resource,
	SdlInputClauses&       clauses,
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
inline void TSdlOwnerClass<Owner, FieldSet>::referencedResources(
	const ISdlResource* const targetResource,
	std::vector<const ISdlResource*>& out_resources) const
{
	static_assert(std::is_base_of_v<ISdlResource, Owner>,
		"Owner class must derive from ISdlResource.");

	const Owner& owner = *(castTo<const Owner>(targetResource));
	for(std::size_t fieldIdx = 0; fieldIdx < m_fields.numFields(); ++fieldIdx)
	{
		m_fields[fieldIdx].ownedResources(owner, out_resources);
	}

	// Find more references in base class
	if(isDerived())
	{
		PH_ASSERT(getBase());
		getBase()->referencedResources(targetResource, out_resources);
	}
}

template<typename Owner, typename FieldSet>
inline std::size_t TSdlOwnerClass<Owner, FieldSet>::numFields() const
{
	return m_fields.numFields();
}

template<typename Owner, typename FieldSet>
inline const SdlField* TSdlOwnerClass<Owner, FieldSet>::getField(const std::size_t index) const
{
	return getOwnedField(index);
}

template<typename Owner, typename FieldSet>
inline std::size_t TSdlOwnerClass<Owner, FieldSet>::numFunctions() const
{
	return m_functions.size();
}

template<typename Owner, typename FieldSet>
inline const SdlFunction* TSdlOwnerClass<Owner, FieldSet>::getFunction(const std::size_t index) const
{
	return index < m_functions.size() ? m_functions[index] : nullptr;
}

template<typename Owner, typename FieldSet>
inline const TSdlOwnedField<Owner>* TSdlOwnerClass<Owner, FieldSet>::getOwnedField(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename Owner, typename FieldSet>
template<typename SdlFieldType>
inline auto TSdlOwnerClass<Owner, FieldSet>::addField(SdlFieldType sdlField)
	-> TSdlOwnerClass&
{
	// More restrictions on the type of SdlFieldType may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, SdlFieldType>,
		"Provided SdlFieldType is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename Owner, typename FieldSet>
template<typename StructType>
inline auto TSdlOwnerClass<Owner, FieldSet>::addStruct(StructType Owner::* const structObjPtr)
	-> TSdlOwnerClass&
{
	return addStruct(structObjPtr, SdlStructFieldStump());
}

template<typename Owner, typename FieldSet>
template<typename StructType>
inline auto TSdlOwnerClass<Owner, FieldSet>::addStruct(
	StructType Owner::* const  structObjPtr,
	const SdlStructFieldStump& structFieldStump)

	-> TSdlOwnerClass&
{
	// TODO: require StructType has getSdlFunction()

	PH_ASSERT(structObjPtr);

	m_fields.addFields(structFieldStump.genFieldSet(structObjPtr));

	return *this;
}

template<typename Owner, typename FieldSet>
template<typename T>
inline auto TSdlOwnerClass<Owner, FieldSet>::addFunction()
	-> TSdlOwnerClass&
{
	// TODO: require T has getSdlFunction()

	m_functions.pushBack(T::getSdlFunction());

	return *this;
}

template<typename Owner, typename FieldSet>
inline void TSdlOwnerClass<Owner, FieldSet>::loadFieldsFromSdl(
	Owner&                  owner, 
	SdlInputClauses&        clauses,
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
inline void TSdlOwnerClass<Owner, FieldSet>::setFieldsToDefaults(Owner& owner) const
{
	for(std::size_t fieldIdx = 0; fieldIdx < m_fields.numFields(); ++fieldIdx)
	{
		const auto& field = m_fields[fieldIdx];

		// Set field to default value regardless of its importance (field importance is for import/export)
		field.setValueToDefault(owner);
	}
}

template<typename Owner, typename FieldSet>
inline void TSdlOwnerClass<Owner, FieldSet>::saveFieldsToSdl(
	const Owner&            owner,
	SdlOutputClauses&       out_clauses,
	const SdlOutputContext& ctx) const
{
	for(std::size_t fieldIdx = 0; fieldIdx < m_fields.numFields(); ++fieldIdx)
	{
		const TSdlOwnedField<Owner>& field = m_fields[fieldIdx];

		SdlOutputClause& clause = out_clauses.createClause();
		sdl::save_field_id(&field, clause);
		field.toSdl(owner, clause, ctx);
	}
}

template<typename Owner, typename FieldSet>
inline auto TSdlOwnerClass<Owner, FieldSet>::description(std::string descriptionStr)
-> TSdlOwnerClass&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename Owner, typename FieldSet>
inline auto TSdlOwnerClass<Owner, FieldSet>::docName(std::string docName)
-> TSdlOwnerClass&
{
	setDocName(std::move(docName));
	return *this;
}

template<typename Owner, typename FieldSet>
template<typename T>
inline auto TSdlOwnerClass<Owner, FieldSet>::baseOn()
-> TSdlOwnerClass&
{
	static_assert(!std::is_same_v<T, Owner>,
		"A SDL class cannot base on itself.");

	setBase<T>();

	// If base cannot be created from class, the derived should be the same
	if(getBase() && !getBase()->allowCreateFromClass())
	{
		allowCreateFromClass(false);
	}

	return *this;
}

template<typename Owner, typename FieldSet>
auto TSdlOwnerClass<Owner, FieldSet>::allowCreateFromClass(const bool allowCreateFromClass)
-> TSdlOwnerClass&
{
	setAllowCreateFromClass(allowCreateFromClass);
	return *this;
}

template<typename Owner, typename FieldSet>
template<typename DstType, typename SrcType>
inline DstType* TSdlOwnerClass<Owner, FieldSet>::castTo(SrcType* const srcInstance) const
{
	try
	{
		return sdl::cast_to<DstType>(srcInstance);
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlException>(
			"input resource is not owned by this class <{}> ({})",
			genPrettyName(), e.whatStr());
	}
}

}// end namespace ph
