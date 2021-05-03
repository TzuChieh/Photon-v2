#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/ValueClause.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/SdlIOUtils.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"

#include <string>
#include <utility>
#include <string_view>
#include <array>
#include <type_traits>

namespace ph
{

template<typename Owner, typename FieldSet = TBasicSdlFieldSet<TOwnedSdlField<Owner>>>
class TOwnerSdlClass : public SdlClass
{
	static_assert(std::is_base_of_v<ISdlResource, Owner>,
		"Owner class must derive from ISdlResource.");

	static_assert(!std::is_abstract_v<Owner> && std::is_default_constructible_v<Owner>,
		"A non-abstract owner class must have a default constructor.");

public:
	TOwnerSdlClass(std::string category, std::string displayName);

	std::shared_ptr<ISdlResource> createResource() const override;

	void initResource(
		ISdlResource&          resource,
		const ValueClause*     clauses,
		std::size_t            numClauses,
		const SdlInputContext& ctx) const override;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	void fromSdl(
		Owner&                 owner,
		const ValueClause*     clauses,
		std::size_t            numClauses,
		const SdlInputContext& ctx);

	void toSdl(
		const Owner& owner,
		std::string* out_sdl,
		std::string& out_message) const;

	const TOwnedSdlField<Owner>* getOwnedField(std::size_t index) const;

	TOwnerSdlClass& addField(std::unique_ptr<TOwnedSdlField<Owner>> field);

private:
	FieldSet m_fields;
};

// In-header Implementations:


}// end namespace ph

#include "DataIO/SDL/Introspect/TOwnerSdlClass.ipp"
