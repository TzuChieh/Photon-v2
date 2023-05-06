#pragma once

#include "SDL/Introspect/field_set_op.h"
#include "SDL/ValueClauses.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"

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
	NoticeReceiver         noticeReceiver)
{
	// Consider to increase the number if not enough
	constexpr std::size_t MAX_FIELD_FLAGS = 64;
	PH_ASSERT_GE(MAX_FIELD_FLAGS, fieldSet.numFields());

	// Zero initialization performed on array elements (defaults to false)
	std::array<bool, MAX_FIELD_FLAGS> isFieldTouched{};

	// For each clause, load them into matching field
	for(std::size_t clauseIdx = 0; clauseIdx < clauses.size();)
	{
		// TODO: check isFieldTouched and warn on duplicating clauses?

		const auto& clause   = clauses[clauseIdx];
		const auto& fieldIdx = fieldSet.findFieldIndex(clause.type, clause.name);
		if(fieldIdx)
		{
			const auto& field = fieldSet[fieldIdx.value()];
			field.fromSdl(owner, clause.payload, ctx);

			isFieldTouched[fieldIdx.value()] = true;

			// Consume the clause once a match is found; no need to increment
			// <clauseIdx> since a new one will fill the empty slot
			clauses.consumeBySwapPop(clauseIdx);
		}
		else
		{
			if constexpr(SHOULD_NOTIFY_REDUNDANT_CLAUSE)
			{
				// Treat a redundant clause input as an optional field
				noticeReceiver(
					"SDL class <" + ctx.genPrettySrcClassName() + "> has no matching field for "
					"input clause <" + clause.genPrettyName() + ">, ignoring",
					EFieldImportance::Optional);
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
				if(importance != EFieldImportance::Optional)
				{
					noticeReceiver(
						"no clause for " + sdl::gen_pretty_name(ctx.getSrcClass(), &field) +
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
	NoticeReceiver         noticeReceiver)
{
	load_fields_from_sdl<Owner, FieldSet, NoticeReceiver, false>(
		owner,
		fieldSet,
		clauses,
		ctx,
		std::move(noticeReceiver));
}

}// end namespace ph::field_set_op
