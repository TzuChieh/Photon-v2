#pragma once

#include "DataIO/SDL/Introspect/field_set_op.h"
#include "DataIO/SDL/ValueClauses.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <array>
#include <utility>

namespace ph::field_set_op
{

template<
	typename Owner,
	typename FieldSet,
	typename NoticeReceiver,
	bool     SHOULD_NOTIFY_REDUNDANT_CLAUSE>
inline void load_fields_from_sdl(
	Owner&                 owner,
	FieldSet&              fieldSet,
	ValueClauses&          clauses,
	const SdlInputContext& ctx,
	NoticeReceiver&&       noticeReceiver)
{
	// Consider to increase the number if not enough
	constexpr std::size_t MAX_FIELD_FLAGS = 64;
	PH_ASSERT_GE(MAX_FIELD_FLAGS, fieldSet.numFields());

	// Zero initialization performed on array elements (defaults to false)
	std::array<bool, MAX_FIELD_FLAGS> isFieldTouched{};

	// For each clause, load them into matching field
	for(std::size_t clauseIdx = 0; clauseIdx < clauses.size();)
	{
		const auto& clause   = clauses[clauseIdx];
		const auto& fieldIdx = fieldSet.findFieldIndex(clause.type, clause.name);
		if(fieldIdx)
		{
			const auto& field = fieldSet[fieldIdx.value()];
			field.fromSdl(owner, clause.payload, ctx);

			isFieldTouched[fieldIdx.value()] = true;

			// Remove the clause once a match is found; no need to increment
			// <clauseIdx> since a new one will fill the empty slot
			clauses.removeBySwapPop(clauseIdx);
		}
		else
		{
			if constexpr(SHOULD_NOTIFY_REDUNDANT_CLAUSE)
			{
				// Treat a redundant clause input as an optional field
				std::forward<NoticeReceiver>(noticeReceiver)(
					"type <" + ctx.genPrettySrcClassName() + "> has no matching field for "
					"clause <" + clause.genPrettyName() + ">, ignoring",
					EFieldImportance::OPTIONAL);
			}

			// No match is found, skip to next clause
			++clauseIdx;
		}
	}

	// Check and process uninitialized fields
	for(std::size_t fieldIdx = 0; fieldIdx < fieldSet.numFields(); ++fieldIdx)
	{
		if(!isFieldTouched[fieldIdx])
		{
			const auto& field = fieldSet[fieldIdx];
			if(field.isFallbackEnabled())
			{
				field.setValueToDefault(owner);

				// Only optional field will be silently set to default
				// (emit notice for other importance levels)
				//
				const auto importance = field.getImportance();
				if(importance != EFieldImportance::OPTIONAL)
				{
					std::forward<NoticeReceiver>(noticeReceiver)(
						"no clause for " + sdl::gen_pretty_name(ctx.srcClass, &field) +
						", defaults to <" + field.valueToString(owner) + ">",
						importance);
				}
			}
			else
			{
				throw SdlLoadError(
					"a clause for value <" + field.genPrettyName() + "> is required");
			}

			// TODO: util for generating class + field info string
		}
	}
}

template<
	typename Owner,
	typename FieldSet,
	typename NoticeReceiver>
inline void load_fields_from_sdl_with_redundant_clauses(
	Owner&                 owner,
	FieldSet&              fieldSet,
	ValueClauses&          clauses,
	const SdlInputContext& ctx,
	NoticeReceiver&&       noticeReceiver)
{
	load_fields_from_sdl<Owner, FieldSet, NoticeReceiver, false>(
		owner,
		fieldSet,
		clauses,
		ctx,
		std::forward<NoticeReceiver>(noticeReceiver));
}

}// end namespace ph::field_set_op
