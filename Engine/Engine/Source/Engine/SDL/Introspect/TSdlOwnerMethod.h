#pragma once

#include "Engine/SDL/Introspect/SdlFunction.h"
#include "Engine/SDL/Definition/ISdlDefaultFunctionDefinition.h"
#include "Engine/SDL/Introspect/FieldSet/TSdlDefaultFieldSet.h"
#include "Engine/SDL/Introspect/TSdlOwnedField.h"

#include <cstddef>
#include <type_traits>

namespace ph
{

/*! @brief SDL binding type for a canonical SDL method.

For a method to be callable in SDL, it must be defined as a C++ functor that takes 
an instance of @p Target as its only input. Other arguments to the method should
be accessed via the functor's member variables. Each member variables then can be 
binded much like the members of a canonical SDL resource. For example,

```cpp
struct ExampleMethod
{
	int methodParam;

	void operator () (Target& target) const
	{
		// your implementation here
	}

	// SDL definition here (i.e., binding `methodParam` and provide docs)
};
```

@tparam MethodStruct Type of the functor.
@tparam Target Type that defines the method. Must be a SDL resource type.
*/
template<typename MethodStruct, typename Target, typename FieldSet = TSdlDefaultFieldSet<TSdlOwnedField<MethodStruct>>>
class TSdlOwnerMethod : public SdlFunction, public ISdlDefaultFunctionDefinition
{
public:
	using OwnerType  = MethodStruct;
	using TargetType = Target;

public:
	TSdlOwnerMethod();

	using SdlFunction::call;

	void call(
		ISdlResource*              resource,
		const SdlNonConstInstance& instance,
		SdlInputClauses&           clauses,
		const SdlInputContext&     ctx) const override;
	
	SdlInstantiated instantiate() const override;
	std::size_t numParams() const override;
	const SdlField* getParam(std::size_t index) const override;
	bool isStatic() const override;

	/*! @brief Call the method on a target instance using a specific functor.
	@param target The target instance the method operates on.
	@param functor The functor to be called.
	@param clauses Input clauses for the call.
	@param ctx Context for the call.
	*/
	void callMethod(
		Target&                    target,
		MethodStruct&              functor,
		SdlInputClauses&           clauses,
		const SdlInputContext&     ctx) const;

	/*! @brief Call the method on a target instance using a type-erased functor instance.
	@param target The target instance the method operates on.
	@param instance The type-erased functor instance.
	@param clauses Input clauses for the call.
	@param ctx Context for the call.
	*/
	void callMethod(
		Target&                    target,
		const SdlNonConstInstance& instance,
		SdlInputClauses&           clauses,
		const SdlInputContext&     ctx) const;

	/*! @brief Load parameters from clauses into a parameter struct.
	@param parameterStruct The struct to load parameters into.
	@param clauses Input clauses containing the parameters.
	@param ctx Context for the loading process.
	*/
	void loadParameters(
		MethodStruct&              parameterStruct,
		SdlInputClauses&           clauses,
		const SdlInputContext&     ctx) const;

	template<typename T>
	TSdlOwnerMethod& addParam(T sdlField);

	/*! @brief Set the name of the method.
	@param nameStr The new name.
	@return `*this` for chaining.
	*/
	auto name(std::string nameStr) -> TSdlOwnerMethod&;

	/*! @brief Set the description of the method.
	@param descriptionStr The new description.
	@return `*this` for chaining.
	*/
	auto description(std::string descriptionStr) -> TSdlOwnerMethod&;

	/*! @brief Set the user specifications for the method.
	@param spec The new user specifications.
	@return `*this` for chaining.
	*/
	auto userSpec(SdlUserSpec spec) -> TSdlOwnerMethod&;

	// TODO: support structs?

private:
	FieldSet m_fields;
};

}// end namespace ph

#include "Engine/SDL/Introspect/TSdlOwnerMethod.ipp"
