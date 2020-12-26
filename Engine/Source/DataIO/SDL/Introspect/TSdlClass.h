#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/ValueClause.h"

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <string_view>
#include <array>
#include <optional>

namespace ph
{

#define PH_SDLCALSS_USE_TRIE false

template<typename Owner>
class TSdlClass : public SdlClass
{
public:
	TSdlClass(std::string category, std::string displayName);

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	void fromSdl(
		Owner&             owner,
		const ValueClause* clauses,
		std::size_t        numClauses,
		std::string&       out_message);

	void toSdl(
		Owner&       owner,
		std::string* out_sdl,
		std::string& out_message) const;

	const TOwnedSdlField<Owner>* getOwnedField(std::size_t index) const;

	TSdlClass& addField(std::unique_ptr<TOwnedSdlField<Owner>> field);

private:
	std::optional<std::size_t> findOwnedSdlFieldIndex(std::string_view typeName, std::string_view fieldName) const;

	std::vector<std::unique_ptr<TOwnedSdlField<Owner>>> m_fields;

	static constexpr int MAX_FIELDS = 64;
};

// In-header Implementations:

template<typename Owner>
inline TSdlClass<Owner>::TSdlClass(std::string category, std::string displayName) :
	SdlClass(std::move(category), std::move(displayName))
{}

template<typename Owner>
inline std::size_t TSdlClass<Owner>::numFields() const
{
	return m_fields.size();
}

template<typename Owner>
inline const SdlField* TSdlClass<Owner>::getField(const std::size_t index) const
{
	return getOwnedField(index);
}

template<typename Owner>
inline const TOwnedSdlField<Owner>* TSdlClass<Owner>::getOwnedField(const std::size_t index) const
{
	return index < m_fields.size() ? m_fields[index].get() : nullptr;
}

template<typename Owner>
inline TSdlClass<Owner>& TSdlClass<Owner>::addField(std::unique_ptr<TOwnedSdlField<Owner>> field)
{
	PH_ASSERT(field);
	PH_ASSERT_LT(m_fields.size(), MAX_FIELDS);
	PH_ASSERT_MSG(findOwnedSdlField(field->getTypeName(), field->getFieldName()) == nullptr,
		"SDL class <" + genPrettyName() + "> already contains field <" field->genPrettyName() + ">");

	m_fields.push_back(std::move(field));

	return *this;
}

template<typename Owner>
inline void TSdlClass<Owner>::fromSdl(
	Owner&                   owner,
	const ValueClause* const clauses,
	const std::size_t        numClauses,
	std::string&             out_message)
{
	PH_ASSERT(clauses);

	// Zero initialization performed on array elements
	std::array<bool, MAX_FIELDS> isFieldTouched{};

	std::string fieldMessages;
	for(std::size_t i = 0; i < numClauses; ++i)
	{
		const auto* const clause = clauses[i];

		PH_ASSERT(clause);
		const auto& fieldIndex = findOwnedSdlFieldIndex(clause->type, clause->name);
		if(fieldIndex)
		{
			const auto& field = m_fields[fieldIndex.value()];
			isFieldTouched[fieldIndex.value()] = true;

			std::string message;
			if(!field->fromSdl(owner, clause->value, message))
			{
				out_message += "Note: SDL class <" + genPrettyName() + ">";
			}
		}
		else
		{
			out_message += "[Note: SDL class <" + genPrettyName() + "> has no matching field "
				"for clause <" + clause->genPrettyName() + ">, ignoring]";
		}
	}
}

template<typename Owner>
inline void TSdlClass<Owner>::toSdl(
	Owner&             owner,
	std::string* const out_sdl,
	std::string&       out_message) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

template<typename Owner>
inline std::optional<std::size_t> TSdlClass<Owner>::findOwnedSdlFieldIndex(
	const std::string_view typeName,
	const std::string_view fieldName) const
{
	PH_ASSERT(!typeName.empty());
	PH_ASSERT(!fieldName.empty());

#if PH_SDLCALSS_USE_TRIE
	PH_ASSERT_UNREACHABLE_SECTION();
	return std::nullopt;
#else
	for(std::size_t i = 0; i < m_fields.size(); ++i)
	{
		const auto& field = m_fields[i];
		if(typeName == field->getTypeName() && fieldName == field->getFieldName())
		{
			return i;
		}
	}
	return std::nullopt;
#endif
}

}// end namespace ph
