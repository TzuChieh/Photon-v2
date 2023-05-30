#include "SDL/SdlInlinePacketInterface.h"
#include "SDL/SdlInputClause.h"
#include "SDL/SdlInputClauses.h"
#include "SDL/SdlOutputClause.h"
#include "SDL/SdlOutputClauses.h"
#include "SDL/Tokenizer.h"
#include "SDL/sdl_exceptions.h"

#include <utility>
#include <vector>

namespace ph
{

SdlInlinePacketInterface::SdlInlinePacketInterface()
	: SdlInlinePacketInterface(Path("./"))
{}

SdlInlinePacketInterface::SdlInlinePacketInterface(const Path& sceneWorkingDirectory)
	: SdlDataPacketInterface(sceneWorkingDirectory)
{}

void SdlInlinePacketInterface::parse(
	std::string_view packetCommand,
	const SdlClass* /* targetClass */,
	std::string_view /* targetName */,
	SdlInputClauses& out_clauses) const
{
	parseClauses(packetCommand, out_clauses);
}

void SdlInlinePacketInterface::generate(
	const SdlOutputClauses& clauses,
	const SdlClass* /* targetClass */,
	std::string_view /* targetName */,
	std::string& out_packetCommand) const
{
	for(std::size_t clauseIdx = 0; clauseIdx < clauses.numClauses(); ++clauseIdx)
	{
		appendSingleClause(clauses[clauseIdx], out_packetCommand);
	}
}

void SdlInlinePacketInterface::parseClauses(std::string_view packetCommand, SdlInputClauses& out_clauses)
{
	static const Tokenizer clausesTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'[', ']'}});

	// OPT: use view
	std::vector<std::string> clauseStrings;
	clausesTokenizer.tokenize(std::string(packetCommand), clauseStrings);

	// TODO: reuse input clause
	for(const auto& clauseString : clauseStrings)
	{
		SdlInputClause clause;
		parseSingleClause(clauseString, clause);
		out_clauses.add(std::move(clause));
	}
}

void SdlInlinePacketInterface::parseSingleClause(std::string_view clauseString, SdlInputClause& out_clause)
{
	static const Tokenizer clauseTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'\"', '\"'}, {'{', '}'}});

	if(clauseString.empty())
	{
		throw SdlLoadError("syntax error: clause string is empty");
	}

	// TODO: tokenize string_view
	std::vector<std::string> tokens;
	clauseTokenizer.tokenize(std::string(clauseString), tokens);
	if(tokens.size() != 3)
	{
		throw_formatted<SdlLoadError>(
			"syntax error: bad number of tokens ({}), expected to be 3",
			tokens.size());
	}

	out_clause.type = tokens[0];
	out_clause.value = tokens[2];
	
	// Parse name and an optional tag
	// tokens[1] contains name and tag, syntax: <name>:<optional-tag>

	const std::string_view nameAndTag = tokens[1];
	const auto colonPos = nameAndTag.find(':');
	if(colonPos == std::string_view::npos)
	{
		out_clause.name = tokens[1];
		out_clause.tag = "";
	}
	else
	{
		out_clause.name = std::string(nameAndTag.substr(0, colonPos + 1));
		out_clause.tag = nameAndTag.substr(colonPos);
	}
}

void SdlInlinePacketInterface::appendSingleClause(
	const SdlOutputClause& clause,
	std::string& out_commandStr)
{
	out_commandStr += '[';
	out_commandStr += clause.type;
	out_commandStr += ' ';
	out_commandStr += clause.name;

	if(clause.hasTag())
	{
		out_commandStr += ": ";
		out_commandStr += clause.tag;
	}

	out_commandStr += ' ';
	out_commandStr += clause.value;
	out_commandStr += ']';
}

}// end namespace ph
