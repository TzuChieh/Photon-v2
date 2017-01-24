#include "FileIO/Tokenizer.h"

#include <iostream>

namespace ph
{

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
	if(source.empty())
	{
		std::cerr << "warning: at Tokenizer::tokenize(), source string is empty" << std::endl;
		return;
	}

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
			i = extractEnclosureSeparatedToken(source, i, ch, out_results);
		}
		else
		{
			i = extractSeparatorSeparatedToken(source, i, out_results);
		}
	}
}

std::size_t Tokenizer::extractSeparatorSeparatedToken(const std::string& source, const std::size_t startIndex, std::vector<std::string>& out_results) const
{
	std::size_t i = startIndex + 1;
	while(i < source.length())
	{
		if(isSeparator(source[i]))
		{
			break;
		}

		i++;
	}

	out_results.push_back(source.substr(startIndex, i - startIndex));

	const std::size_t nextIndex = i + 1;
	return nextIndex;
}

std::size_t Tokenizer::extractEnclosureSeparatedToken(const std::string& source, const std::size_t startIndex, 
                                                      const char enclosureStart, std::vector<std::string>& out_results) const
{
	std::size_t i = startIndex + 1;
	while(i < source.length())
	{
		if(isEnclosurePair(enclosureStart, source[i]))
		{
			break;
		}

		i++;
	}

	// plus 1 since we don't want to include the enclosure characters as part of a token
	out_results.push_back(source.substr(startIndex + 1, i - (startIndex + 1)));

	const std::size_t nextIndex = i + 1;
	return nextIndex;
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