#pragma once

#include "DataIO/SDL/Introspect/TOwnerSdlClass.h"
#include "DataIO/SDL/Introspect/field_set_op.h"

#include <type_traits>

namespace ph
{

template<typename Owner, typename FieldSet>
inline TOwnerSdlClass<Owner, FieldSet>::TOwnerSdlClass(std::string category, std::string displayName) :

	SdlClass(std::move(category), std::move(displayName)),

	m_fields()
{}

template<typename Owner, typename FieldSet>
inline std::shared_ptr<ISdlResource> TOwnerSdlClass<Owner, FieldSet>::createResource() const
{
	if constexpr(!std::is_abstract_v<Owner>)
	{
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
inline const TOwnedSdlField<Owner>* TOwnerSdlClass<Owner, FieldSet>::getOwnedField(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename Owner, typename FieldSet>
template<typename T>
inline TOwnerSdlClass<Owner, FieldSet>& TOwnerSdlClass<Owner, FieldSet>::addField(T field)
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(field));

	return *this;
}

template<typename Owner, typename FieldSet>
inline void TOwnerSdlClass<Owner, FieldSet>::fromSdl(
	Owner&                   owner,
	const ValueClause* const clauses,
	const std::size_t        numClauses,
	const SdlInputContext&   ctx)
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
inline void TOwnerSdlClass<Owner, FieldSet>::toSdl(
	const Owner&       owner,
	std::string* const out_sdl,
	std::string&       out_message) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
