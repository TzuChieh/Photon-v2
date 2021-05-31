#pragma once

#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"

#include <cstddef>
#include <type_traits>

namespace ph
{

template<typename MethodStruct, typename TargetType>
class TSdlMethod : public SdlFunction
{
	static_assert(std::is_default_constructible_v<MethodStruct> && !std::is_abstract_v<MethodStruct>,
		"MethodStruct must be default constructible.");

	static_assert(std::is_invocable_v<MethodStruct, TargetType>,
		"MethodStruct must contain operator() that takes a TargetType.");

public:
	explicit TSdlMethod(std::string name);

	void call(
		ISdlResource*          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const override;

	std::size_t numParams() const override;
	const SdlField* getParam(std::size_t index) const override;

	void callMethod(
		TargetType&            targetType,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const;

	void loadParameters(
		MethodStruct&          parameterStruct,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const;

	template<typename T>
	TSdlMethod& addParam(T sdlField);

	// TODO: support structs?

private:
	TBasicSdlFieldSet<TOwnedSdlField<MethodStruct>> m_fields;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlMethod.ipp"
