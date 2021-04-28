#pragma once

#include "DataIO/SDL/Introspect/field_set_op.h"
#include "DataIO/SDL/ValueClause.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Common/assertion.h"

#include <array>
#include <utility>

namespace ph::field_set_op
{

template<typename Owner, typename FieldSet, typename NoticeReceiver>
inline void load_fields_from_sdl(
	Owner&                   owner,
	FieldSet&                fieldSet,
	const ValueClause* const clauses,
	const std::size_t        numClauses,
	SdlInputContext&         ctx,
	NoticeReceiver&&         noticeReceiver)
{
	PH_ASSERT(clauses);

	// Consider to increase the number if not enough
	constexpr std::size_t MAX_FIELD_FLAGS = 64;
	PH_ASSERT_GE(MAX_FIELD_FLAGS, m_fields.numFields());

	// Zero initialization performed on array elements (defaults to false)
	std::array<bool, MAX_FIELD_FLAGS> isFieldTouched{};

	// For each clause, load them into matching field
	for(std::size_t i = 0; i < numClauses; ++i)
	{
		const auto* const clause = clauses[i];

		PH_ASSERT(clause);
		const auto& fieldIndex = m_fields.findFieldIndex(clause->type, clause->name);
		if(fieldIndex)
		{
			auto& field = m_fields[fieldIndex.value()];
			isFieldTouched[fieldIndex.value()] = true;

			field->fromSdl(owner, clause->value, ctx);
		}
		else
		{
			// Treat a redundant clause input as an optional field
			std::forward<NoticeReceiver>(noticeReceiver)(
				"type <" + genPrettyName() + "> has no matching field for "
				"clause <" + clause->genPrettyName() + ">, ignoring",
				EFieldImportance::OPTIONAL);
		}
	}

	// Check and process uninitialized fields
	for(std::size_t i = 0; i < m_fields.size(); ++i)
	{
		if(!isFieldTouched[i])
		{
			auto& field = m_fields[i];
			if(field->isFallbackEnabled())
			{
				field->setValueToDefault(owner);

				// Only optional field will be silently set to default
				// (emit notice for other importance levels)
				const auto importance = field->getImportance();
				if(importance != EFieldImportance::OPTIONAL)
				{
					std::forward<NoticeReceiver>(noticeReceiver)(
						"no clause for " + SdlIOUtils::genPrettyName(this, &field) +
						", defaults to <" + field->valueToString(owner) + ">",
						importance);
				}
			}
			else
			{
				throw SdlLoadError(
					"a clause for value <" + field->genPrettyName() + "> is required");
			}

			// TODO: util for generating class + field info string
		}
	}
}

}// end namespace ph::field_set_op
