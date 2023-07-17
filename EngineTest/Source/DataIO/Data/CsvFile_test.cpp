#include "config.h"

#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>

#include <gtest/gtest.h>

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

	// With trailing comma
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
}
