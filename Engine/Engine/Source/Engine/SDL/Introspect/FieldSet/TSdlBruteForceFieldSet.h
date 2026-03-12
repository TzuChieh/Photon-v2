#pragma once

#include "Engine/SDL/Introspect/SdlField.h"
#include "Engine/Utility/TArrayVector.h"
#include "Engine/Utility/IMoveOnly.h"
#include "Engine/SDL/sdl_exceptions.h"

#include <Common/assertion.h>
#include <Common/config.h>

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

/*! @brief A set of fields, with basic functionalities.

This class accepts polymorphic field types. 
This class finds a field using brute-force method.

For faster field lookup, you may add a trie or hash map variant, see `TSdlSortedFieldSet` for example.
Currently there are not so many fields in a single owner type, so we think current implementation
should be good enough.
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
	/*! @brief Get the number of fields in the set.
	*/
	inline std::size_t numFields() const
	{
		return m_fields.size();
	}

	/*! @brief Get a field by index.
	@param index Index of the field.
	@return Pointer to the field, or nullptr if index is out of bounds.
	*/
	inline const BaseFieldType* getField(const std::size_t index) const
	{
		return index < m_fields.size() ? &(*this)[index] : nullptr;
	}

	/*! @brief Add a field to the set.
	@param field The field to add.
	@return `*this` for chaining.
	@throw SdlException If field name is duplicated or the set is full.
	*/
	template<typename T>
	inline auto addField(T field)
	-> TSdlBruteForceFieldSet&
	{
		ensureWeCanAddField(field);

		m_fields.pushBack(std::make_unique<T>(std::move(field)));

		return *this;
	}

	/*! @brief Add multiple fields from another set.
	@param otherFields The set of fields to add.
	@return `*this` for chaining.
	*/
	template<typename OtherBaseFieldType, std::size_t OTHER_MAX_FIELDS>
	inline auto addFields(TSdlBruteForceFieldSet<OtherBaseFieldType, OTHER_MAX_FIELDS> otherFields)
	-> TSdlBruteForceFieldSet&
	{
		static_assert(std::is_base_of_v<BaseFieldType, OtherBaseFieldType>,
			"Incoming field type must derive from the field type that this set stores.");

		for(std::size_t i = 0; i < otherFields.numFields(); ++i)
		{
			ensureWeCanAddField(otherFields[i]);

			m_fields.pushBack(std::move(otherFields.m_fields[i]));
		}

		return *this;
	}

	/*! @brief Find the index of a field by name.
	@param fieldName Name of the field to find.
	@return The index of the field, or std::nullopt if not found.
	*/
	inline std::optional<std::size_t> findFieldIndex(const std::string_view fieldName) const
	{
		PH_ASSERT(!fieldName.empty());

		for(std::size_t i = 0; i < m_fields.size(); ++i)
		{
			const auto& field = m_fields[i];
			if(fieldName == field->getFieldName())
			{
				return i;
			}
		}
		return std::nullopt;
	}

	/*! @brief Get a field by index.
	*/
	inline const BaseFieldType& operator [] (const std::size_t index) const
	{
		return *(m_fields[index]);
	}

private:
	template<typename T>
	inline void ensureWeCanAddField(const T& field)
	{
		static_assert(std::is_base_of_v<BaseFieldType, T>,
			"Cannot add a field that is not derived from the field type of the set.");

		const bool isFieldUnique = !findFieldIndex(field.getFieldName());
		const bool hasMoreSpace  = !m_fields.isFull();

		if(!isFieldUnique)
		{
			throw_formatted<SdlException>("field set already contains field <{}>", field.genPrettyName());
		}

		if(!hasMoreSpace)
		{
			throw_formatted<SdlException>("field set is full, consider increase its size to {}", m_fields.size());
		}
	}

	TArrayVector<std::unique_ptr<BaseFieldType>, MAX_FIELDS> m_fields{};
};

}// end namespace ph
