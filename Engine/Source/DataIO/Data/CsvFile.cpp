#include "DataIO/Data/CsvFile.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"
#include "DataIO/Stream/FormattedTextInputStream.h"
#include "DataIO/Stream/FormattedTextOutputStream.h"

#include <utility>
#include <sstream>

namespace ph
{

std::size_t CsvFileRow::numValues() const
{
	return m_values.size();
}

const std::string& CsvFileRow::getValue(const std::size_t index) const
{
	PH_ASSERT_LT(index, numValues());
	return m_values[index];
}

std::string& CsvFileRow::getValue(const std::size_t index)
{
	PH_ASSERT_LT(index, numValues());
	return m_values[index];
}

CsvFileRow& CsvFileRow::addValue(std::string value)
{
	m_values.push_back(std::move(value));
	return *this;
}

CsvFile& CsvFile::load(const Path& csvFile)
{
	// Content should not skip any characters--we need newlines to distinguish between rows
	std::istringstream content;
	{
		std::string allText;
		FormattedTextInputStream(csvFile).readAll(&allText);
		content = std::istringstream(allText);
	}
	
	std::vector<std::string> rowTexts;
	{
		std::string rowText;
		while(std::getline(content, rowText))
		{
			rowTexts.push_back(rowText);
		}
	}

	for(const std::string& rowText : rowTexts)
	{
		std::istringstream rowContent(rowText);

		std::string value;
		CsvFileRow row;
		while(std::getline(rowContent, value, ','))
		{
			// Note: `getline()` will not ignore any whitespace (excluding delimiters)
			row.addValue(value);
		}

		// Check for a trailing comma with no value after it
		if(!rowContent && value.empty())
		{
			// If there was a trailing comma then add an empty value
			row.addValue(value);
		}

		addRow(row);
	}

	return *this;
}

const CsvFile& CsvFile::save(const Path& csvFile) const
{
	FormattedTextOutputStream file(csvFile);
	for(std::size_t ri = 0; ri < m_rows.size(); ++ri)
	{
		const CsvFileRow& row = m_rows[ri];
		for(std::size_t vi = 0; vi < row.numValues(); ++vi)
		{
			file.writeString(row.getValue(vi));

			const bool isLastValue = (vi == row.numValues() - 1);
			if(!isLastValue)
			{
				file.writeChar(',');
			}
		}

		file.writeChar('\n');
	}

	return *this;
}

std::size_t CsvFile::numRows() const
{
	return m_rows.size();
}

const CsvFileRow& CsvFile::getRow(const std::size_t index) const
{
	PH_ASSERT_LT(index, numRows());
	return m_rows[index];
}

CsvFileRow& CsvFile::getRow(const std::size_t index)
{
	PH_ASSERT_LT(index, numRows());
	return m_rows[index];
}

CsvFile& CsvFile::addRow(CsvFileRow row)
{
	m_rows.push_back(std::move(row));
	return *this;
}

}// end namespace ph
