#pragma once

#include <Common/Utility/string_utils.h>

#include <cstddef>
#include <vector>
#include <string>

namespace ph
{

class Path;

class CsvFileRow final
{
public:
	std::size_t numValues() const;
	const std::string& getValue(std::size_t index) const;
	std::string& getValue(std::size_t index);
	CsvFileRow& addValue(std::string value);

	template<typename NumberType>
	NumberType getValue(std::size_t index) const;

private:
	std::vector<std::string> m_values;
};

/*!
A very basic .csv file IO utility. Empty rows are not skipped.
Does not handle multiline quoted field (a quoted string value with newline characters) currently.
*/
class CsvFile final
{
public:
	inline CsvFile() = default;

	/*! @brief Loads a .csv file. Effectively calls `load()`.
	*/
	explicit CsvFile(const Path& csvFile);

	CsvFile& load(const Path& csvFile);
	const CsvFile& save(const Path& csvFile) const;
	std::size_t numRows() const;
	const CsvFileRow& getRow(std::size_t index) const;
	CsvFileRow& getRow(std::size_t index);
	CsvFile& addRow(CsvFileRow row);

private:
	std::vector<CsvFileRow> m_rows;
};

template<typename NumberType>
inline NumberType CsvFileRow::getValue(const std::size_t index) const
{
	return string_utils::parse_number<NumberType>(getValue(index));
}

}// end namespace ph
