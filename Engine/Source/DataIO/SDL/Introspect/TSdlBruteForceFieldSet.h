#pragma once

#include "DataIO/SDL/Introspect/SdlField.h"
#include "Common/assertion.h"
#include "Utility/TArrayAsVector.h"
#include "Common/config.h"
#include "Utility/IMoveOnly.h"

#include <cstddef>
#include <type_traits>
#include <array>
#include <memory>
#include <string_view>
#include <optional>
#include <utility>
#include <string>

namespace ph
{

// TODO: trie or unordered_map variant
// TODO: add a field set concept

/*! @brief A set of fields, with basic functionalities.

This class accepts polymorphic field types. 
This class finds a field using brute-force method.
*/
template<typename BaseFieldType, std::size_t MAX_FIELDS = PH_SDL_MAX_FIELDS>
class TSdlBruteForceFieldSet final : private IMoveOnly
{
	static_assert(std::is_base_of_v<SdlField, BaseFieldType>,
		"Field type must derive from SdlField.");

	template<typename OtherBaseFieldType, std::size_t OTHER_MAX_FIELDS>
	friend class TSdlBruteForceFieldSet;

public:
	using FieldType = BaseFieldType;

public:
	inline std::size_t numFields() const
	{
		return m_fields.size();
	}

	inline const BaseFieldType* getField(const std::size_t index) const
	{
		return index < m_fields.size() ? &(*this)[index] : nullptr;
	}

	template<typename T>
	inline auto addField(T field)
	-> TSdlBruteForceFieldSet&
	{
		if(canAddField(field))
		{
			m_fields.pushBack(std::make_unique<T>(std::move(field)));
		}

		return *this;
	}

	template<typename OtherBaseFieldType, std::size_t OTHER_MAX_FIELDS>
	inline auto addFields(TSdlBruteForceFieldSet<OtherBaseFieldType, OTHER_MAX_FIELDS> fields)
	-> TSdlBruteForceFieldSet&
	{
		static_assert(std::is_base_of_v<BaseFieldType, OtherBaseFieldType>,
			"Incoming field type must derive from the field type that this set stores.");

		for(std::size_t i = 0; i < fields.numFields(); ++i)
		{
			if(!canAddField(fields[i]))
			{
				break;
			}

			m_fields.pushBack(std::move(fields.m_fields[i]));
		}

		return *this;
	}

	inline std::optional<std::size_t> findFieldIndex(
		const std::string_view typeName,
		const std::string_view fieldName) const
	{
		PH_ASSERT(!typeName.empty());
		PH_ASSERT(!fieldName.empty());

		for(std::size_t i = 0; i < m_fields.size(); ++i)
		{
			const auto& field = m_fields[i];
			if(typeName == field->getTypeName() && fieldName == field->getFieldName())
			{
				return i;
			}
		}
		return std::nullopt;
	}

	inline const BaseFieldType& operator [] (const std::size_t index) const
	{
		return *(m_fields[index]);
	}

private:
	TArrayAsVector<std::unique_ptr<BaseFieldType>, MAX_FIELDS> m_fields;

	template<typename T>
	inline bool canAddField(const T& field)
	{
		static_assert(std::is_base_of_v<BaseFieldType, T>,
			"Cannot add a field that is not derived from the field type of the set.");

		const bool isFieldUnique = !findFieldIndex(field.getTypeName(), field.getFieldName());
		const bool hasMoreSpace  = !m_fields.isFull();

		PH_ASSERT_MSG(isFieldUnique,
			"field set already contains field <" + field.genPrettyName() + ">");

		PH_ASSERT_MSG(hasMoreSpace,
			"field set is full, consider increase its size "
			"(> " + std::to_string(m_fields.size()) + ")");

		return isFieldUnique && hasMoreSpace;
	}
};

}// end namespace ph
