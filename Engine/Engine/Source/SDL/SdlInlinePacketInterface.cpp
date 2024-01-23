#include "SDL/SdlInlinePacketInterface.h"
#include "SDL/SdlInputClause.h"
#include "SDL/SdlInputClauses.h"
#include "SDL/SdlOutputClause.h"
#include "SDL/SdlOutputClauses.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/ISdlDataPacketGroup.h"
#include "SDL/Tokenizer.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_parser.h"

#include <Utility/string_utils.h>

#include <utility>
#include <algorithm>

namespace ph
{

SdlInlinePacketInterface::SdlInlinePacketInterface()
	: SdlInlinePacketInterface(-1, '\t')
{}

SdlInlinePacketInterface::SdlInlinePacketInterface(int clauseIndentAmount, char clauseIndentChar)
	: SdlDataPacketInterface()
	, m_clausePrefix()
{
	if(!string_utils::is_whitespace(clauseIndentChar))
	{
		throw_formatted<SdlException>(
			"{} is not a valid whitespace for indentation.",
			clauseIndentChar);
	}

	if(clauseIndentAmount >= 0)
	{
		// Always start each clause in a new line if indentation was specified
		m_clausePrefix = '\n';

		if(clauseIndentAmount > 0)
		{
			m_clausePrefix.resize(1 + clauseIndentAmount);
			std::fill(m_clausePrefix.begin() + 1, m_clausePrefix.end(), clauseIndentChar);
		}
	}
}

void SdlInlinePacketInterface::parse(
	std::string_view packetCommand,
	const SdlInputContext& ctx,
	std::string_view /* targetName */,
	ISdlResource* /* targetInstance */,
	SdlInputClauses& out_clauses) const
{
	static const Tokenizer clausesTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'[', ']'}});

	// OPT: use view
	std::vector<std::string> clauseStrings;
	clausesTokenizer.tokenize(std::string(packetCommand), clauseStrings);

	parseClauses(clauseStrings, ctx, out_clauses);
}

void SdlInlinePacketInterface::generate(
	const SdlOutputClauses& clauses,
	const SdlOutputContext& /* ctx */,
	std::string_view /* targetName */,
	const ISdlResource* /* targetInstance */,
	std::string& out_packetCommand) const
{
	for(std::size_t clauseIdx = 0; clauseIdx < clauses.numClauses(); ++clauseIdx)
	{
		if(clauses[clauseIdx].isEmpty)
		{
			continue;
		}

		out_packetCommand += m_clausePrefix;
		appendSingleClause(clauses[clauseIdx], out_packetCommand);
	}
}

void SdlInlinePacketInterface::parseClauses(
	TSpanView<std::string> clauseStrings,
	const SdlInputContext& ctx,
	SdlInputClauses& out_clauses)
{
	static const Tokenizer clauseTokenizer(
		{' ', '\t', '\n', '\r'}, 
		{{'"', '"'}, {'{', '}'}});

	// TODO: reuse input clause
	for(const auto& clauseString : clauseStrings)
	{
		if(clauseString.empty())
		{
			throw SdlLoadError(
				"syntax error: clause string is empty");
		}

		// TODO: tokenize string_view
		std::vector<std::string> clauseTokens;
		clauseTokenizer.tokenize(std::string(clauseString), clauseTokens);

		if(clauseTokens.size() == 1)
		{
			std::string_view dataPacketName = string_utils::trim(clauseTokens[0]);
			if(dataPacketName.empty())
			{
				throw_formatted<SdlLoadError>(
					"syntax error: data packet name is empty");
			}

			// If current SDL dialect allows named data packet, then it does not make sense to pass
			// a context without source data packets.
			PH_ASSERT(ctx.getSrcDataPackets());

			const SdlInputClauses* dataPacket = ctx.getSrcDataPackets()->get(dataPacketName);
			if(!dataPacket)
			{
				throw_formatted<SdlLoadError>(
					"no data packet is named {}", dataPacketName);
			}

			out_clauses.add(*dataPacket);
		}
		else
		{
			SdlInputClause clause;
			parseSingleClause(clauseTokens, clause);
			out_clauses.add(std::move(clause));
		}
	}
}

void SdlInlinePacketInterface::parseSingleClause(
	TSpanView<std::string> clauseTokens,
	SdlInputClause& out_clause)
{
	const auto tokens = clauseTokens;
	if(tokens.size() < 2)
	{
		throw_formatted<SdlLoadError>(
			"syntax error: incomplete clause detected, only {} tokens are specified",
			tokens.size());
	}

	// The first token is always type
	out_clause.type = tokens[0];
	
	// Parse name and an optional tag
	// `tokens[1]` contains name and tag, syntax: <name>:<optional-tag>

	const std::string_view nameAndTag = tokens[1];
	const auto colonPos = nameAndTag.find(':');
	if(colonPos == std::string_view::npos)
	{
		out_clause.name = tokens[1];
		out_clause.tag = "";
	}
	else
	{
		out_clause.name = nameAndTag.substr(0, colonPos);
		out_clause.tag = nameAndTag.substr(colonPos + 1);
	}

	// Parse the value section of a SDL clause
	switch(tokens.size())
	{
	case 2:
		// Value section omitted or being empty, e.g., ""
		out_clause.value = "";
		break;

	case 3:
		// "@" signifies a following reference,
		// e.g., `@Ref`
		if(tokens[2].starts_with('@'))
		{
			out_clause.isReference = true;
			out_clause.value = sdl_parser::get_reference(tokens[2]);
		}
		// Otherwise, just a ordinary value
		else
		{
			out_clause.isReference = false;
			out_clause.value = tokens[2];
		}
		break;

	case 4:
		// "@" signifies a following reference,
		// e.g., `@ Ref`, `@"Ref"`
		//         ^ with whitespaces
		if(tokens[2] == "@")
		{
			out_clause.isReference = true;
			out_clause.value = tokens[3];
		}
		// Otherwise, it is an error
		else
		{
			throw_formatted<SdlLoadError>(
				"syntax error: unexpected token sequence <{} {}> specified in clause",
				tokens[2], tokens[3]);
		}
		break;

	default:
		throw_formatted<SdlLoadError>(
			"syntax error: too many tokens ({}) are specified for the clause",
			tokens.size());
		break;
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
		out_commandStr += ':';
		out_commandStr += clause.tag;
	}

	out_commandStr += ' ';
	if(clause.isReference)
	{
		out_commandStr += '@';
	}

	// Clause values know how to group its content properly (e.g., by double quotes), 
	// and we should not add any prefix or suffix here.
	out_commandStr += clause.value;

	out_commandStr += ']';
}

}// end namespace ph
