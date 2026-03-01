#pragma once

#include "Engine/SDL/sdl_fwd.h"

#include <cstddef>
#include <string_view>

namespace ph
{

class ISdlInstantiable
{
public:
	virtual ~ISdlInstantiable() = default;

	/*!
	@return An instance. The actual type of the instance depends on the SDL definition
	of this instantiable. Depending on the SDL definition, the instance may be null
	(e.g., we cannot instantiate an abstract class).
	*/
	virtual SdlInstantiated instantiate() const = 0;

	/*!
	@return Number of fields in this instantiable.
	*/
	virtual std::size_t numFields() const = 0;

	/*!
	@return Field at `index`. `index` must <= `numFields()`.
	*/
	virtual const SdlField* getField(std::size_t index) const = 0;

	/*!
	@return Type name of this instantiable.
	*/
	virtual std::string_view getTypeName() const = 0;

	/*!
	@return A description of this instantiable.
	*/
	virtual std::string_view getDescription() const = 0;

	// TODO: some way to cooperate with the `CSdlInstance` concept
};

}// end namespace ph
