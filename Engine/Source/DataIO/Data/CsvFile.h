#pragma once

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

private:
	std::vector<std::string> m_values;
};

/*!
A very basic .csv file IO utility. Empty lines are not skipped.
Does not handle multiline quoted field (a quoted string value with newline characters) currently.
*/
class CsvFile final
{
public:
	CsvFile& load(const Path& csvFile);
	const CsvFile& save(const Path& csvFile) const;
	std::size_t numRows() const;
	const CsvFileRow& getRow(std::size_t index) const;
	CsvFileRow& getRow(std::size_t index);
	CsvFile& addRow(CsvFileRow row);

private:
	std::vector<CsvFileRow> m_rows;
};

}// end namespace ph
