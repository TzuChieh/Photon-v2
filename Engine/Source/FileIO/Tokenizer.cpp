#include "FileIO/SDL/Tokenizer.h"

#include <iostream>

namespace ph
{

Tokenizer::Tokenizer(const std::vector<char>& separators) : 
	Tokenizer(separators, {})
{}

Tokenizer::Tokenizer(const std::vector<std::pair<char, char>>& enclosures) : 
	Tokenizer({}, enclosures)
{}

Tokenizer::Tokenizer(const std::vector<char>& separators, const std::vector<std::pair<char, char>>& enclosures) :
	m_separators(separators), m_enclosures(enclosures)
{
	if(separators.empty() && enclosures.empty())
	{
		std::cerr << "warning: at Tokenizer::Tokenizer(), neither separator nor enclosure is specified" << std::endl;
	}
}

void Tokenizer::tokenize(const std::string& source, std::vector<std::string>& out_results) const
{
	std::size_t i = 0;
	while(i < source.length())
	{
		const char ch = source[i];
		if(isSeparator(ch))
		{
			i++;
		}
		else if(isEnclosureStart(ch))
		{
			i = extractEnclosureSeparatedToken(source, i + 1, ch, out_results) + 1;
		}
		else
		{
			i = extractSeparatorSeparatedToken(source, i, out_results);
		}
	}
}

std::size_t Tokenizer::extractSeparatorSeparatedToken(const std::string& source, const std::size_t tokenStartIndex, 
                                                      std::vector<std::string>& out_results) const
{
	std::size_t i = tokenStartIndex;
	while(i < source.length())
	{
		if(isSeparator(source[i]) || isEnclosureStart(source[i]))
		{
			break;
		}

		i++;
	}

	const std::string token = source.substr(tokenStartIndex, i - tokenStartIndex);
	if(!token.empty())
	{
		out_results.push_back(token);
	}

	const std::size_t tokenEndIndexExclusive = i;
	return tokenEndIndexExclusive;
}

std::size_t Tokenizer::extractEnclosureSeparatedToken(const std::string& source, const std::size_t tokenStartIndex,
                                                      const char enclosureStart, std::vector<std::string>& out_results) const
{
	bool isEnclosurePairFound = false;
	std::size_t i = tokenStartIndex;
	while(i < source.length())
	{
		if(isEnclosurePair(enclosureStart, source[i]))
		{
			isEnclosurePairFound = true;
			break;
		}

		i++;
	}

	if(isEnclosurePairFound)
	{
		const std::string token = source.substr(tokenStartIndex, i - tokenStartIndex);
		if(!token.empty())
		{
			out_results.push_back(token);
		}
	}
	else
	{
		std::cerr << "warning: at Tokenizer::extractEnclosureSeparatedToken(), enclosure pair not found" << std::endl;
	}

	const std::size_t tokenEndIndexExclusive = i;
	return tokenEndIndexExclusive;
}

bool Tokenizer::isSeparator(const char character) const
{
	for(const char separater : m_separators)
	{
		if(character == separater)
		{
			return true;
		}
	}

	return false;
}

bool Tokenizer::isEnclosureStart(const char startCh) const
{
	for(const auto& enclosure : m_enclosures)
	{
		if(startCh == enclosure.first)
		{
			return true;
		}
	}

	return false;
}

bool Tokenizer::isEnclosurePair(const char startCh, const char endCh) const
{
	for(const auto& enclosure : m_enclosures)
	{
		if(startCh == enclosure.first && endCh == enclosure.second)
		{
			return true;
		}
	}

	return false;
}

}// end namespace ph