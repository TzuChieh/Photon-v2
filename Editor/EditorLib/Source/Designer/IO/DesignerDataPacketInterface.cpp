#include "Designer/IO/DesignerDataPacketInterface.h"

#include "ThirdParty/NLohmannJSON.h"

#include <DataIO/FileSystem/Path.h>
#include <DataIO/Stream/BinaryFileInputStream.h>
#include <DataIO/Stream/BinaryFileOutputStream.h>
#include <DataIO/Stream/FormattedTextInputStream.h>
#include <DataIO/Stream/FormattedTextOutputStream.h>
#include <Common/assertion.h>
#include <Utility/string_utils.h>
#include <SDL/SdlResourceLocator.h>
#include <SDL/sdl_exceptions.h>
#include <SDL/SdlInputClauses.h>
#include <SDL/SdlOutputClauses.h>
#include <SDL/Introspect/SdlInputContext.h>
#include <SDL/Introspect/SdlOutputContext.h>
#include <SDL/Introspect/SdlClass.h>
#include <SDL/sdl_helpers.h>
#include <DataIO/io_exceptions.h>
#include <DataIO/FileSystem/ResourceIdentifier.h>

#include <utility>

namespace ph::editor
{

DesignerDataPacketInterface::DesignerDataPacketInterface()
	: SdlDataPacketInterface()
{}

void DesignerDataPacketInterface::parse(
	std::string_view packetCommand,
	const SdlInputContext& ctx,
	std::string_view const targetName,
	ISdlResource* const targetInstance,
	SdlInputClauses& out_clauses) const
{
	const SdlClass* targetClass = ctx.getSrcClass();

	// Packet command is a bundled resource identifier (for the packet file)
	const Path packetFile = SdlResourceLocator(ctx).toPath(packetCommand);

	const auto& fileExt = packetFile.getExtension();
	if(fileExt == ".pddp")
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
	else if(fileExt == ".pddpa")
	{
		try
		{
			FormattedTextInputStream stream(packetFile);

			std::string text;
			stream.readAll(&text);
			parseTextPacket(text, out_clauses);
		}
		catch(const IOException& e)
		{
			throw_formatted<SdlLoadError>(
				"error while loading packet {} (for target class: {}, name: {}) -> {}",
				packetFile,
				sdl::gen_pretty_name(targetClass),
				targetName.empty() ? "(unavailable)" : targetName,
				e.whatStr());
		}
	}
	else
	{
		throw_formatted<SdlLoadError>(
			"unknown designer packet extension {}",
			fileExt);
	}
}

void DesignerDataPacketInterface::generate(
	const SdlOutputClauses& clauses,
	const SdlOutputContext& ctx,
	std::string_view targetName,
	const ISdlResource* const targetInstance,
	std::string& out_packetCommand) const
{
	const SdlClass* targetClass = ctx.getSrcClass();
	if(!targetClass || targetName.empty())
	{
		throw_formatted<SdlSaveError>(
			"unable to generate designer packet with missing info (for target class: {}, name: {})",
			sdl::gen_pretty_name(targetClass),
			targetName.empty() ? "(unavailable)" : targetName);
	}

	std::string valueInfoBuffer;
	valueInfoBuffer.reserve(256);

	nlohmann::json jsonObj;
	for(std::size_t clauseIdx = 0; clauseIdx < clauses.numClauses(); ++clauseIdx)
	{
		const SdlOutputClause& clause = clauses[clauseIdx];

		valueInfoBuffer.clear();
		valueInfoBuffer += clause.type;
		valueInfoBuffer += ", ";
		valueInfoBuffer += clause.name;
		if(clause.hasTag())
		{
			valueInfoBuffer += ", ";
			valueInfoBuffer += clause.tag;
		}

		jsonObj[valueInfoBuffer] = clause.value;
	}

	// Filename: <target-type>_<target-name>.<ext> (ignore angle brackets)
	const auto packetFilename = targetClass->getTypeName() + "_" + std::string(targetName) + ".pddpa";

	const Path packetDirectory = ctx.getWorkingDirectory() / "designer_packet";
	packetDirectory.createDirectory();

	const Path packetFile = packetDirectory / packetFilename;

	try
	{
		FormattedTextOutputStream stream(packetFile);
		//stream.writeString(nlohmann::to_string(jsonObj));
		stream.writeString(jsonObj.dump(1, '\t'));
	}
	catch(const IOException& e)
	{
		throw_formatted<SdlSaveError>(
			"error while writing packet {} (for target class: {}, name: {}) -> {}",
			packetFile,
			sdl::gen_pretty_name(targetClass),
			targetName.empty() ? "(unavailable)" : targetName,
			e.whatStr());
	}

	const std::string bundleIdentifier = SdlResourceLocator(ctx)
		.toBundleIdentifier(packetFile).getIdentifier();

	// Packet command is a bundle resource identifier, quoted (for the packet file)
	out_packetCommand += '"';
	out_packetCommand.append(bundleIdentifier);
	out_packetCommand += '"';
}

void DesignerDataPacketInterface::parseTextPacket(const std::string& text, SdlInputClauses& out_clauses)
{
	nlohmann::json jsonObj = nlohmann::json::parse(text);
	for(auto& itemEntry : jsonObj.items())
	{
		SdlInputClause clause;
		parseSingleClause(itemEntry.key(), itemEntry.value(), clause);
		out_clauses.add(std::move(clause));
	}
}

void DesignerDataPacketInterface::parseSingleClause(
	std::string_view valueInfoStr,
	std::string_view valueStr,
	SdlInputClause& out_clause)
{
	using namespace string_utils;

	// Value info is of the form: 
	// <type-name>, <value-name>, <tag-name> (ignore angle brackets)
	// where the tag-name part may be optional
	//
	out_clause.type = trim(next_token(valueInfoStr, &valueInfoStr, ","));
	out_clause.name = trim(next_token(valueInfoStr, &valueInfoStr, ","));
	out_clause.tag = trim(next_token(valueInfoStr, &valueInfoStr, ","));

	out_clause.value = trim(valueStr);
}

}// end namespace ph::editor
