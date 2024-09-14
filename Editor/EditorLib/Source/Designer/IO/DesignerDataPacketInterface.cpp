#include "Designer/IO/DesignerDataPacketInterface.h"

#include "ThirdParty/NLohmannJSON.h"

#include <DataIO/FileSystem/Path.h>
#include <DataIO/FileSystem/Filesystem.h>
#include <DataIO/Stream/BinaryFileInputStream.h>
#include <DataIO/Stream/BinaryFileOutputStream.h>
#include <DataIO/Stream/FormattedTextInputStream.h>
#include <DataIO/Stream/FormattedTextOutputStream.h>
#include <Common/assertion.h>
#include <Common/Utility/string_utils.h>
#include <SDL/SdlResourceLocator.h>
#include <SDL/sdl_exceptions.h>
#include <SDL/SdlInputClauses.h>
#include <SDL/SdlOutputClauses.h>
#include <SDL/Introspect/SdlInputContext.h>
#include <SDL/Introspect/SdlOutputContext.h>
#include <SDL/Introspect/SdlClass.h>
#include <SDL/sdl_helpers.h>
#include <SDL/sdl_parser.h>
#include <Common/io_exceptions.h>
#include <DataIO/FileSystem/ResourceIdentifier.h>

#include <utility>
#include <format>

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

	// Packet command is a bundled resource identifier (for the packet file), quoted.
	// Remove quotes to obtain the identifier:
	std::string_view identifier = sdl_parser::trim_double_quotes(packetCommand);

	const Path packetFile = SdlResourceLocator(ctx).toPath(identifier);

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
		if(clause.isEmpty)
		{
			continue;
		}

		// Designer does not support saving SDL named target (e.g., reference). This need to be
		// reported and fail early, otherwise saved data may be corrupted or cumbersome to recover.
		// A better way to prevent this would be to disallow binding to named targets during
		// SDL definition stage.
		if(clause.valueType != ESdlClauseValue::General)
		{
			throw_formatted<SdlSaveError>(
				"Attempting to directly save SDL reference, this is not a feature supported by "
				"designer data packet interface. Clause type: {}, name: {}, reference: {}",
				clause.type, clause.name, clause.value);
		}

		valueInfoBuffer.clear();
		valueInfoBuffer += clause.type;
		valueInfoBuffer += ", ";
		valueInfoBuffer += clause.name;
		if(clause.hasTag())
		{
			valueInfoBuffer += ", ";
			valueInfoBuffer += clause.tag;
		}

		// Clause value may be enclosed by double quotes in order to group its content. This is
		// not necessary as a JSON string already adds double quotes. This removes any potentially
		// redundant double quotes from the clause value.
		jsonObj[valueInfoBuffer] = sdl_parser::trim_double_quotes(clause.value);
	}

	// Filename: <target-type>_<target-name>.<ext> (ignore angle brackets)
	const auto packetFilename = std::format("{}_{}.pddpa", targetClass->getTypeName(), targetName);

	const Path packetDirectory = ctx.getWorkingDirectory() / "DesignerPacket";
	Filesystem::createDirectories(packetDirectory);

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

	// Packet command is a bundle resource identifier (for the packet file), quoted
	out_packetCommand += '"';
	out_packetCommand.append(bundleIdentifier);
	out_packetCommand += '"';
}

void DesignerDataPacketInterface::parseTextPacket(const std::string& text, SdlInputClauses& out_clauses)
{
	nlohmann::json jsonObj = nlohmann::json::parse(text);
	for(auto&& jsonEntry : jsonObj.items())
	{
		try
		{
			// For now JSON encoded packets are all string values
			const auto& jsonValue = jsonEntry.value().get_ref<const nlohmann::json::string_t&>();

			SdlInputClause clause;
			parseSingleClause(jsonEntry.key(), jsonValue, clause);
			out_clauses.add(std::move(clause));
		}
		catch(const nlohmann::json::exception& e)
		{
			throw_formatted<SdlLoadError>(
				"JSON encoded packet error: {}", e.what());
		}
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
