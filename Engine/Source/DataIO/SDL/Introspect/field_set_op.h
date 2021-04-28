#pragma once

#include <cstddef>
#include <string>

namespace ph
{

class ValueClause;
class SdlInputContext;

}// end namespace ph

namespace ph::field_set_op
{

struct NoOpNoticeReceiver final
{
	inline void operator (std::string noticeMsg, EFieldImportance importance) const
	{}
};

template<typename Owner, typename FieldSet, typename NoticeReceiver>
void load_fields_from_sdl(
	Owner&             owner,
	FieldSet&          fieldSet,
	const ValueClause* clauses,
	std::size_t        numClauses,
	SdlInputContext&   ctx,
	NoticeReceiver&&   noticeReceiver = NoOpNoticeReceiver());

}// end namespace ph::field_set_op

#include "DataIO/SDL/Introspect/field_set_op.ipp"
