#pragma once

#include <vector>
#include <utility>
#include <string>
#include <string_view>

namespace ph
{

class InputPrototype final
{
public:
	std::vector<std::pair<std::string_view, std::string>> typeNamePairs;

	void addInteger(const std::string& name);
	void addReal(const std::string& name);
	void addString(const std::string& name);
	void addVector3(const std::string& name);
	void addQuaternion(const std::string& name);
	void addRealArray(const std::string& name);
	void addVector3Array(const std::string& name);

	std::string toString() const;

	// TODO: prototype for shared resources

	// TODO: auto resource extraction
	// (extract resources from InputPacket automatically)
};

}// namespace ph
