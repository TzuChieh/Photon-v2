#pragma once

namespace ph::sdl::introspect_utils
{

template<typename Owner, typename FieldSet>
inline void load_field_from_sdl(
	Owner&                   owner,
	FieldSet&                fieldSet,
	const ValueClause* const clauses,
	const std::size_t        numClauses,
	SdlInputContext&         ctx)
{

}

}// end namespace ph
