#pragma once

#include "DataIO/SDL/Introspect/field_set_utils.h"
#include "DataIO/SDL/ValueClause.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Common/assertion.h"

namespace ph::field_set_utils
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

}// end namespace ph::field_set_utils
