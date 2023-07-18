#include "config.h"

#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>

#include <gtest/gtest.h>

#include <cstdint>

using namespace ph;

TEST(CsvFileTest, LoadFile)
{
	// Single row
	{
		CsvFile file;
		file.load(Path(PH_TEST_RESOURCE_PATH("CSV/single_row.csv")));
		EXPECT_EQ(file.numRows(), 1);

		const CsvFileRow& row = file.getRow(0);
		EXPECT_EQ(row.numValues(), 3);

		EXPECT_STREQ(row.getValue(0).c_str(), "aaa");
		EXPECT_STREQ(row.getValue(1).c_str(), "bbb");
		EXPECT_STREQ(row.getValue(2).c_str(), "ccc");
	}

	// Multi-row
	{
		CsvFile file;
		file.load(Path(PH_TEST_RESOURCE_PATH("CSV/multi_row.csv")));
		EXPECT_EQ(file.numRows(), 3);

		const CsvFileRow& row0 = file.getRow(0);
		EXPECT_EQ(row0.numValues(), 2);

		EXPECT_STREQ(row0.getValue(0).c_str(), "aaa");
		EXPECT_STREQ(row0.getValue(1).c_str(), "bbb");

		const CsvFileRow& row1 = file.getRow(1);
		EXPECT_EQ(row1.numValues(), 2);

		EXPECT_STREQ(row1.getValue(0).c_str(), "ccc");
		EXPECT_STREQ(row1.getValue(1).c_str(), "ddd");

		const CsvFileRow& row2 = file.getRow(2);
		EXPECT_EQ(row2.numValues(), 2);

		EXPECT_STREQ(row2.getValue(0).c_str(), "eee");
		EXPECT_STREQ(row2.getValue(1).c_str(), "fff");
	}

	// Get numbers (read as numeric values)
	{
		CsvFile file;
		file.load(Path(PH_TEST_RESOURCE_PATH("CSV/numbers.csv")));
		EXPECT_EQ(file.numRows(), 3);

		const CsvFileRow& row0 = file.getRow(0);
		EXPECT_EQ(row0.numValues(), 3);

		EXPECT_EQ(row0.getValue<int>(0), 0);
		EXPECT_EQ(row0.getValue<int>(1), 3);
		EXPECT_EQ(row0.getValue<int>(2), 4);

		const CsvFileRow& row1 = file.getRow(1);
		EXPECT_EQ(row1.numValues(), 3);

		EXPECT_EQ(row1.getValue<std::int32_t>(0), 12345);
		EXPECT_EQ(row1.getValue<std::int32_t>(1), 678);
		EXPECT_EQ(row1.getValue<std::int32_t>(2), 9101112);

		const CsvFileRow& row2 = file.getRow(2);
		EXPECT_EQ(row2.numValues(), 3);

		// This row has large numbers
		EXPECT_EQ(row2.getValue<std::uint32_t>(0), 0xFFFFFFFFUL);
		EXPECT_EQ(row2.getValue<std::uint32_t>(1), 0xAAAAAAAAUL);
		EXPECT_EQ(row2.getValue<std::uint64_t>(2), 0xABCDABCDABCDABCDULL);
	}

	// With trailing comma (special case)
	{
		CsvFile file;
		file.load(Path(PH_TEST_RESOURCE_PATH("CSV/trailing_comma.csv")));
		EXPECT_EQ(file.numRows(), 1);

		const CsvFileRow& row = file.getRow(0);
		EXPECT_EQ(row.numValues(), 4);

		EXPECT_STREQ(row.getValue(0).c_str(), "aaa");
		EXPECT_STREQ(row.getValue(1).c_str(), "bbb");
		EXPECT_STREQ(row.getValue(2).c_str(), "ccc");

		// The last value is empty--after a trailing comma
		EXPECT_STREQ(row.getValue(3).c_str(), "");
	}

	// With trailing empty line (special case)
	{
		CsvFile file;
		file.load(Path(PH_TEST_RESOURCE_PATH("CSV/trailing_empty_line.csv")));

		// The last line is empty and should not be considered a row (not ended with newline)
		EXPECT_EQ(file.numRows(), 3);

		const CsvFileRow& row0 = file.getRow(0);
		EXPECT_EQ(row0.numValues(), 1);
		EXPECT_STREQ(row0.getValue(0).c_str(), "1");

		const CsvFileRow& row1 = file.getRow(1);
		EXPECT_EQ(row1.numValues(), 1);
		EXPECT_STREQ(row1.getValue(0).c_str(), "2");

		const CsvFileRow& row2 = file.getRow(2);
		EXPECT_EQ(row2.numValues(), 1);
		EXPECT_STREQ(row2.getValue(0).c_str(), "3");
	}

	// With empty rows (special case)
	{
		CsvFile file;
		file.load(Path(PH_TEST_RESOURCE_PATH("CSV/empty_rows.csv")));

		// Empty lines (ended with newline) should be considered a valid row.
		// The last line is empty and should not be considered a row (not ended with newline)
		EXPECT_EQ(file.numRows(), 6);

		const CsvFileRow& row0 = file.getRow(0);
		EXPECT_EQ(row0.numValues(), 1);
		EXPECT_STREQ(row0.getValue(0).c_str(), "1");

		// This line is empty (should contain no value)
		const CsvFileRow& row1 = file.getRow(1);
		EXPECT_EQ(row1.numValues(), 0);

		const CsvFileRow& row2 = file.getRow(2);
		EXPECT_EQ(row2.numValues(), 1);
		EXPECT_STREQ(row2.getValue(0).c_str(), "2");

		const CsvFileRow& row3 = file.getRow(3);
		EXPECT_EQ(row3.numValues(), 1);
		EXPECT_STREQ(row3.getValue(0).c_str(), "3");

		// This line is empty (should contain no value)
		const CsvFileRow& row4 = file.getRow(4);
		EXPECT_EQ(row4.numValues(), 0);

		// This line is empty (should contain no value)
		const CsvFileRow& row5 = file.getRow(5);
		EXPECT_EQ(row5.numValues(), 0);
	}
}
