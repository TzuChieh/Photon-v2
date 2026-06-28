#include "engine_test_util.h"

#include <Engine/SDL/SdlInlinePacketInterface.h>
#include <Engine/SDL/SdlDataPacketCollection.h>
#include <Engine/SDL/SdlInputClause.h>
#include <Engine/SDL/SdlInputClauses.h>
#include <Engine/SDL/Introspect/SdlInputContext.h>
#include <Engine/DataIO/FileSystem/Path.h>

#include <gtest/gtest.h>

#include <utility>

using namespace ph;

TEST(SdlInlinePacketInterfaceTest, ParsesSinglePersistentTargetNameClause)
{
	SdlInlinePacketInterface packetInterface;
	SdlInputClauses clauses;

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, nullptr, &workingDirectory);

	packetInterface.parse("[object-array values @single]", ctx, "", nullptr, clauses);
	ASSERT_EQ(clauses.size(), 1);
	EXPECT_EQ(clauses[0].type, "object-array");
	EXPECT_EQ(clauses[0].name, "values");
	EXPECT_EQ(clauses[0].valueType, ESdlClauseValue::PersistentTargetName);
	EXPECT_EQ(clauses[0].value, "single");
}

TEST(SdlInlinePacketInterfaceTest, ParsesBracketedReferenceListAsGeneralValue)
{
	SdlInlinePacketInterface packetInterface;
	SdlInputClauses clauses;

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, nullptr, &workingDirectory);

	// Braced value should stay as one general-value clause payload.
	packetInterface.parse("[object-array values {@left @right}]", ctx, "", nullptr, clauses);
	ASSERT_EQ(clauses.size(), 1);
	EXPECT_EQ(clauses[0].valueType, ESdlClauseValue::General);
	EXPECT_EQ(clauses[0].value, "@left @right");
}

TEST(SdlInlinePacketInterfaceTest, ParsesEmptyBracedValueAsEmptyGeneralValue)
{
	SdlInlinePacketInterface packetInterface;
	SdlInputClauses clauses;

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, nullptr, &workingDirectory);

	packetInterface.parse("[object-array values {}]", ctx, "", nullptr, clauses);
	ASSERT_EQ(clauses.size(), 1);
	EXPECT_EQ(clauses[0].valueType, ESdlClauseValue::General);
	EXPECT_TRUE(clauses[0].value.empty());
}

TEST(SdlInlinePacketInterfaceTest, ParsesQuotedResourceIdentifierWithSpaces)
{
	SdlInlinePacketInterface packetInterface;
	SdlInputClauses clauses;

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, nullptr, &workingDirectory);

	packetInterface.parse(
		"[PRI image-file \":PH_PICTURE_pictures/albedo texture.png\"]",
		ctx,
		"",
		nullptr,
		clauses);
	ASSERT_EQ(clauses.size(), 1);
	EXPECT_EQ(clauses[0].type, "PRI");
	EXPECT_EQ(clauses[0].name, "image-file");
	EXPECT_EQ(clauses[0].valueType, ESdlClauseValue::General);
	EXPECT_EQ(clauses[0].value, ":PH_PICTURE_pictures/albedo texture.png");
}

TEST(SdlInlinePacketInterfaceTest, ExpandsNamedDataPacketIntoClauses)
{
	SdlInputClauses packet;
	packet.add("integer", "a", "1");
	packet.add("integer", "b", "2");

	SdlDataPacketCollection packets;
	packets.addUnique(std::move(packet), "my-packet");

	SdlInlinePacketInterface packetInterface;
	SdlInputClauses clauses;

	Path workingDirectory(".");
	SdlInputContext ctx(nullptr, &packets, &workingDirectory);

	// `$my-packet` should be expanded into packet contents.
	packetInterface.parse("[$my-packet]", ctx, "", nullptr, clauses);
	ASSERT_EQ(clauses.size(), 2);
	EXPECT_EQ(clauses[0].type, "integer");
	EXPECT_EQ(clauses[0].name, "a");
	EXPECT_EQ(clauses[0].value, "1");
	EXPECT_EQ(clauses[1].type, "integer");
	EXPECT_EQ(clauses[1].name, "b");
	EXPECT_EQ(clauses[1].value, "2");
}
