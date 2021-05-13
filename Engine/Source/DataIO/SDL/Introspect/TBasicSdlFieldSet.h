#pragma once

#include "DataIO/SDL/Introspect/SdlField.h"
#include "Common/assertion.h"
#include "Utility/TFixedSizeVector.h"

#include <cstddef>
#include <type_traits>
#include <array>
#include <memory>
#include <string_view>
#include <optional>
#include <utility>

namespace ph
{

// TODO: trie or unordered_map variant
// TODO: add a field set concept

/*! @brief A set of fields, with basic functionalities.

This class accepts polymorphic field types. 
This class finds a field using brute-force method.
*/
template<typename BaseFieldType, std::size_t MAX_FIELDS = 64>
class TBasicSdlFieldSet final
{
	static_assert(std::is_base_of_v<SdlField, BaseFieldType>,
		"Field type must derive from SdlField.");

public:
	using FieldType = BaseFieldType;

public:
	inline TBasicSdlFieldSet() :
		m_fields(), m_numFields(0)
	{}

	inline std::size_t numFields() const
	{
		return m_numFields;
	}

	inline const BaseFieldType* getField(const std::size_t index) const
	{
		return index < m_numFields ? m_fields[index].get() : nullptr;
	}

	template<typename T>
	inline TBasicSdlFieldSet& addField(T field)
	{
		static_assert(std::is_base_of_v<BaseFieldType, T>,
			"Cannot add a field that is not derived from the field type of the set.");

		PH_ASSERT_LT(m_numFields, m_fields.size());
		PH_ASSERT_MSG(!findFieldIndex(field.getTypeName(), field.getFieldName()),
			"field set already contains field <" field.genPrettyName() + ">");

		if(m_numFields < m_fields.size())
		{
			m_fields.push_back(std::make_unique<T>(std::move(field)));
			++m_numFields;
		}

		return *this;
	}

	inline std::optional<std::size_t> findFieldIndex(
		const std::string_view typeName,
		const std::string_view fieldName)
	{
		PH_ASSERT(!typeName.empty());
		PH_ASSERT(!fieldName.empty());

		for(std::size_t i = 0; i < m_fields.size(); ++i)
		{
			const auto& field = m_fields[i];
			if(typeName == field.getTypeName() && fieldName == field.getFieldName())
			{
				return i;
			}
		}
		return std::nullopt;
	}

	inline const BaseFieldType& operator [] (const std::size_t index) const
	{
		PH_ASSERT_LT(index, m_numFields);

		return *(m_fields[index]);
	}

private:
	std::array<std::unique_ptr<BaseFieldType>, MAX_FIELDS> m_fields;
	std::size_t m_numFields;
};

}// end namespace ph
