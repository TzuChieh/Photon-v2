#pragma once

#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "DataIO/SDL/Introspect/TSdlBruteForceFieldSet.h"
#include "DataIO/SDL/Introspect/TSdlOwnedField.h"

#include <cstddef>
#include <type_traits>

namespace ph
{

/*! @brief SDL binding type for a canonical SDL method.

For a method to be callable in SDL, it must be defined as a C++ functor that takes 
an instance of @p TargetType as its only input. Other arguments to the method should
be accessed via the functor's member variables. Each member variables then can be 
binded much like the members of a canonical SDL resource.

@tparam MethodStruct Type of the functor.
@tparam TargetType Type of the SDL resource class that defines the method.
*/
template<typename MethodStruct, typename TargetType>
class TSdlMethod : public SdlFunction
{
public:
	using OwnerType = MethodStruct;

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

	auto description(std::string descriptionStr) -> TSdlMethod&;

	// TODO: support structs?

private:
	TSdlBruteForceFieldSet<TSdlOwnedField<MethodStruct>> m_fields;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlMethod.ipp"
