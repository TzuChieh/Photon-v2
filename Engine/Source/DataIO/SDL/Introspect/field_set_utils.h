#pragma once

#include <cstddef>

namespace ph
{

class ValueClause;
class SdlInputContext;
class Logger;

}// end namespace ph

namespace ph::field_set_utils
{

template<typename Owner, typename FieldSet>
void load_field_from_sdl(
	Owner&             owner,
	FieldSet&          fieldSet,
	const ValueClause* clauses,
	std::size_t        numClauses,
	SdlInputContext&   ctx);

	// TODO: logger?

}// end namespace ph::field_set_utils

#include "DataIO/SDL/Introspect/field_set_utils.ipp"
