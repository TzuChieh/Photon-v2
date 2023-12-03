#pragma once

#include "SDL/sdl_fwd.h"
#include "SDL/Introspect/EFieldImportance.h"

#include <cstddef>
#include <string>

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
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx,
	NoticeReceiver         noticeReceiver = NoOpNoticeReceiver());

template<
	typename Owner,
	typename FieldSet,
	typename NoticeReceiver>
void load_fields_from_sdl_with_redundant_clauses(
	Owner&                 owner,
	FieldSet&              fieldSet,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx,
	NoticeReceiver         noticeReceiver = NoOpNoticeReceiver());

}// end namespace ph::field_set_op

#include "SDL/Introspect/field_set_op.ipp"
