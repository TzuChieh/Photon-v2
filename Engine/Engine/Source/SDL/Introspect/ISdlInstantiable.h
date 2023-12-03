#pragma once

#include "SDL/sdl_fwd.h"

#include <cstddef>
#include <string_view>

namespace ph
{

class ISdlInstantiable
{
public:
	virtual ~ISdlInstantiable() = default;

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
