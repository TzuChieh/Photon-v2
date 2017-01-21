#pragma once

#include <vector>
#include <string>
#include <utility>

namespace ph
{

class Tokenizer final
{
public:
	Tokenizer(const std::vector<char>& separators, const std::vector<std::pair<char, char>>& enclosures);

	void tokenize(const std::string& source, std::vector<std::string>& out_results) const;

private:
	std::vector<char> m_separators;
	std::vector<std::pair<char, char>> m_enclosures;

	std::size_t extractSeparatorSeparatedToken(const std::string& source, const std::size_t startIndex, std::vector<std::string>& out_results) const;
	std::size_t extractEnclosureSeparatedToken(const std::string& source, const std::size_t startIndex, const char enclosureStart, 
	                                           std::vector<std::string>& out_results) const;
	bool isSeparator(const char character) const;
	bool isEnclosureStart(const char startCh) const;
	bool isEnclosurePair(const char startCh, const char endCh) const;
};

}// end namespace ph