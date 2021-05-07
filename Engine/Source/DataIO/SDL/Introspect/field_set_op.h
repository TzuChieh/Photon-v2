#pragma once

#include "DataIO/SDL/Introspect/EFieldImportance.h"

#include <cstddef>
#include <string>

namespace ph
{

class ValueClauses;
class SdlInputContext;

}// end namespace ph

namespace ph::field_set_op
{

struct NoOpNoticeReceiver final
{
	inline void operator () (std::string noticeMsg, EFieldImportance importance) const
	{}
};

template<
	typename Owner, 
	typename FieldSet, 
	typename NoticeReceiver, 
	bool     SHOULD_NOTIFY_REDUNDANT_CLAUSE = true>
void load_fields_from_sdl(
	Owner&                 owner,
	FieldSet&              fieldSet,
	ValueClauses&          clauses,
	const SdlInputContext& ctx,
	NoticeReceiver&&       noticeReceiver = NoOpNoticeReceiver());

template<
	typename Owner,
	typename FieldSet,
	typename NoticeReceiver>
void load_fields_from_sdl_with_redundant_clauses(
	Owner&                 owner,
	FieldSet&              fieldSet,
	ValueClauses&          clauses,
	const SdlInputContext& ctx,
	NoticeReceiver&&       noticeReceiver = NoOpNoticeReceiver());

}// end namespace ph::field_set_op

#include "DataIO/SDL/Introspect/field_set_op.ipp"
