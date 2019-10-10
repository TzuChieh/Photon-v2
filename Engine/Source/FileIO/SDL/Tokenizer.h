#pragma once

#include <vector>
#include <string>
#include <utility>

namespace ph
{

class Tokenizer final
{
public:
	explicit Tokenizer(const std::vector<char>& separators);
	explicit Tokenizer(const std::vector<std::pair<char, char>>& enclosures);
	Tokenizer(const std::vector<char>& separators, const std::vector<std::pair<char, char>>& enclosures);

	// Tokenize the input string using specified rules (separators & enclosures).
	// The resulting tokens will not include any separator and enclosure characters 
	// or any empty strings.
	// Also notice that source string's starting and ending locations are treated 
	// as implicit separators; for example, the source string will be returned as a
	// token if no separator is provided and no enclosures are found.
	void tokenize(const std::string& source, std::vector<std::string>& out_results) const;

private:
	std::vector<char> m_separators;
	std::vector<std::pair<char, char>> m_enclosures;

	std::size_t extractSeparatorSeparatedToken(const std::string& source, const std::size_t startIndex, 
                                               std::vector<std::string>& out_results) const;
	std::size_t extractEnclosureSeparatedToken(const std::string& source, const std::size_t startIndex, const char enclosureStart, 
	                                           std::vector<std::string>& out_results) const;
	bool isSeparator(const char character) const;
	bool isEnclosureStart(const char startCh) const;
	bool isEnclosurePair(const char startCh, const char endCh) const;
};

}// end namespace ph
