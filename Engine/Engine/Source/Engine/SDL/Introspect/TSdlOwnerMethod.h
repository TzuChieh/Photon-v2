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
		ISdlResource*          resource,
		const SdlInstantiated* instantiated,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const override;
	
	SdlInstantiated instantiate() const override;
	std::size_t numParams() const override;
	const SdlField* getParam(std::size_t index) const override;
	bool isStatic() const override;

	void callMethod(
		Target&                target,
		MethodStruct&          functor,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	void callMethod(
		Target&                target,
		const SdlInstantiated& instantiated,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	void loadParameters(
		MethodStruct&          parameterStruct,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	template<typename T>
	TSdlOwnerMethod& addParam(T sdlField);

	auto name(std::string nameStr) -> TSdlOwnerMethod&;
	auto description(std::string descriptionStr) -> TSdlOwnerMethod&;
	auto userSpec(SdlUserSpec spec) -> TSdlOwnerMethod&;

	// TODO: support structs?

private:
	FieldSet m_fields;
};

}// end namespace ph

#include "Engine/SDL/Introspect/TSdlOwnerMethod.ipp"
