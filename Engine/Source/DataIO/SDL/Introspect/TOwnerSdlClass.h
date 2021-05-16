#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"

#include <string>
#include <utility>
#include <string_view>
#include <array>
#include <type_traits>

namespace ph
{

class ValueClauses;
class SdlInputContext;
class SdlStructFieldStump;

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
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const override;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	void fromSdl(
		Owner&                 owner,
		ValueClauses&          clauses,
		const SdlInputContext& ctx);

	void toSdl(
		const Owner& owner,
		std::string* out_sdl,
		std::string& out_message) const;

	const TOwnedSdlField<Owner>* getOwnedField(std::size_t index) const;

	template<typename T>
	TOwnerSdlClass& addField(T sdlField);

	template<typename T>
	TOwnerSdlClass& addStruct(T Owner::* structObjPtr);

	template<typename T>
	TOwnerSdlClass& addStruct(
		T Owner::*                 structObjPtr,
		const SdlStructFieldStump& structFieldStump);

private:
	FieldSet m_fields;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TOwnerSdlClass.ipp"
