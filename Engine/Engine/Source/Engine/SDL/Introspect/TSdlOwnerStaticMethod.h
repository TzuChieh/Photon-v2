#pragma once

#include "Engine/SDL/Introspect/SdlFunction.h"
#include "Engine/SDL/Definition/ISdlDefaultFunctionDefinition.h"
#include "Engine/SDL/Introspect/FieldSet/TSdlDefaultFieldSet.h"
#include "Engine/SDL/Introspect/TSdlOwnedField.h"

#include <cstddef>
#include <type_traits>

namespace ph
{

/*! @brief SDL binding type for a canonical SDL static method.

For a static method to be callable in SDL, it must be defined as a C++ functor that takes 
no argument. Other arguments to the method should be accessed via the functor's member variables.
Each member variables then can be binded much like the members of a canonical SDL resource.
For example,

```cpp
struct ExampleStaticMethod
{
	int methodParam;

	void operator () () const
	{
		// your implementation here
	}

	// SDL definition here (i.e., binding `methodParam` and provide docs)
};
```

Comparing to `TSdlOwnerMethod`, SDL static method is more flexible as it does not require a SDL
resource as input and does not require creating an instance first to call upon. Modifying a SDL
resource is still possible by explicitly defining a reference to the resource as input.

@tparam MethodStruct Type of the functor.
*/
template<typename MethodStruct, typename FieldSet = TSdlDefaultFieldSet<TSdlOwnedField<MethodStruct>>>
class TSdlOwnerStaticMethod : public SdlFunction, public ISdlDefaultFunctionDefinition
{
public:
	using OwnerType = MethodStruct;

public:
	TSdlOwnerStaticMethod();

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

	void callStaticMethod(
		MethodStruct&          functor,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	void callStaticMethod(
		const SdlInstantiated& instantiated,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	void loadParameters(
		MethodStruct&          parameterStruct,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	template<typename T>
	TSdlOwnerStaticMethod& addParam(T sdlField);

	auto name(std::string nameStr) -> TSdlOwnerStaticMethod&;
	auto description(std::string descriptionStr) -> TSdlOwnerStaticMethod&;
	auto userSpec(SdlUserSpec spec) -> TSdlOwnerStaticMethod&;

	// TODO: support structs?

private:
	FieldSet m_fields;
};

}// end namespace ph

#include "Engine/SDL/Introspect/TSdlOwnerStaticMethod.ipp"
