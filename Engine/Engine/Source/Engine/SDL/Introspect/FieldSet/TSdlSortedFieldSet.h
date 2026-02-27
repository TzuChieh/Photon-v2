#pragma once

#include "Engine/Utility/IMoveOnly.h"
#include "Engine/Utility/TSortedVector.h"
#include "Engine/SDL/Introspect/FieldSet/TSdlBruteForceFieldSet.h"
#include "Engine/SDL/sdl_exceptions.h"

#include <Common/logging.h>

#include <array>
#include <algorithm>
#include <limits>
#include <string>

namespace ph
{

/*! @brief Enhances `TSdlBruteForceFieldSet` with a sorted map for faster field lookup.
*/
template<typename BaseFieldType, std::size_t MAX_FIELDS = PH_SDL_MAX_FIELDS, std::size_t MAX_ENCODE_LEN = 16>
class TSdlSortedFieldSet final : private IMoveOnly
{
public:
	static_assert(MAX_ENCODE_LEN > 0);

	using FieldType = BaseFieldType;

	template<typename OtherBaseFieldType, std::size_t OTHER_MAX_FIELDS, std::size_t OTHER_MAX_ENCODE_LEN>
	friend class TSdlSortedFieldSet;

public:
	std::size_t numFields() const
	{
		return m_fields.numFields();
	}

	const BaseFieldType* getField(const std::size_t index) const
	{
		return index < m_fields.numFields() ? &(*this)[index] : nullptr;
	}

	template<typename T>
	auto addField(T newField)
	-> TSdlSortedFieldSet&
	{
		m_fields.addField(newField);

		FieldInfo newFieldInfo(newField.getFieldName());
		if(!m_fieldInfos.addUniqueValue(newFieldInfo))
		{
			throw_formatted<SdlException>(
				"Cannot add field <{}>, check if it is duplicated or the encoded prefix ({}) is of "
				"insufficient length.", newField.genPrettyName(), newFieldInfo.toString());
		}

		return *this;
	}

	template<typename OtherBaseFieldType, std::size_t OTHER_MAX_FIELDS, std::size_t OTHER_MAX_ENCODE_LEN>
	auto addFields(TSdlSortedFieldSet<OtherBaseFieldType, OTHER_MAX_FIELDS, OTHER_MAX_ENCODE_LEN> otherFields)
	-> TSdlSortedFieldSet&
	{
		static_assert(std::is_base_of_v<BaseFieldType, OtherBaseFieldType>,
			"Incoming field type must derive from the field type that this set stores.");

		const auto numOldFields = m_fields.numFields();
		m_fields.addFields(std::move(otherFields.m_fields));

		// If brute force field set does not complain, then we can safely add all field infos
		for(std::size_t fi = numOldFields; fi < m_fields.numFields(); ++fi)
		{
			// Re-encode with potentially a different encode length
			m_fieldInfos.addUniqueValue(FieldInfo(m_fields[fi].getFieldName()));
		}

		PH_ASSERT_EQ(m_fields.numFields(), m_fieldInfos.size());

		return *this;
	}

	std::optional<std::size_t> findFieldIndex(const std::string_view fieldName) const
	{
		PH_ASSERT(!fieldName.empty());

		return m_fieldInfos.indexOfValue(FieldInfo(fieldName));
	}

	const BaseFieldType& operator [] (const std::size_t index) const
	{
		return m_fields[index];
	}

private:
	struct FieldInfo
	{
		std::array<uint8, MAX_ENCODE_LEN> encodedPrefix;

		/*!
		Construct information for lookup.
		*/
		explicit FieldInfo(const std::string_view fieldName)
			: encodedPrefix(encodePrefix(fieldName))
		{}

		std::string toString() const
		{
			std::string str = "len=" + std::to_string(encodedPrefix[0]);
			str += ", prefix=";
			for(std::size_t i = 1; i < MAX_ENCODE_LEN; ++i)
			{
				str += std::to_string(encodedPrefix[i]) + ";";
			}
			return str;
		}

		bool operator == (const FieldInfo& other) const = default;

		static std::array<uint8, MAX_ENCODE_LEN> encodePrefix(const std::string_view fieldName)
		{
			std::array<uint8, MAX_ENCODE_LEN> result{};

			// First entry stores length, so names of different lengths can be handled naturally
			// by `FieldInfoComparator`
			PH_ASSERT_LE(fieldName.size(), std::numeric_limits<uint32>::max());
			result[0] = fieldName.size();

			// Stores remaining chars as much as possible
			const auto maxChars = std::min(MAX_ENCODE_LEN - 1, fieldName.size());
			for(std::size_t ci = 0; ci < maxChars; ++ci)
			{
				result[ci + 1] = static_cast<uint8>(fieldName[ci]);
			}

			return result;
		}
	};

	struct FieldInfoComparator
	{
		bool operator () (const FieldInfo& a, const FieldInfo& b) const
		{
			return a.encodedPrefix < b.encodedPrefix;
		}
	};

	TSdlBruteForceFieldSet<BaseFieldType, MAX_FIELDS> m_fields;
	TSortedVector<FieldInfo, FieldInfoComparator> m_fieldInfos;
};

}// end namespace ph
