#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"
#include "Utility/TArrayAsVector.h"
#include "Common/config.h"

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

// TODO: allow inheritance of functions?
template<typename Owner, typename FieldSet = TBasicSdlFieldSet<TOwnedSdlField<Owner>>>
class TOwnerSdlClass : public SdlClass
{
public:
	using OwnerType = Owner;

public:
	explicit TOwnerSdlClass(std::string displayName);

	std::shared_ptr<ISdlResource> createResource() const override;

	void initResource(
		ISdlResource&          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const override;

	void call(
		std::string_view       funcName,
		ISdlResource*          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const override;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;
	std::size_t numFunctions() const override;
	const SdlFunction* getFunction(std::size_t index) const override;

	void fromSdl(
		Owner&                 owner,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const;

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

	/*! @brief Adds a function that can later be called.
	*/
	template<typename T>
	TOwnerSdlClass& addFunction(const T* sdlFunction);

private:
	using FunctionSet = TArrayAsVector<const SdlFunction*, PH_SDL_MAX_FUNCTIONS>;

	FieldSet    m_fields;
	FunctionSet m_functions;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TOwnerSdlClass.ipp"
