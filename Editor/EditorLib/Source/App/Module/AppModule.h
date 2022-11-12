#pragma once

#include <Utility/exception.h>

#include <string>

namespace ph::editor
{

class ModuleAttachmentInfo;

class ModuleException : public RuntimeException
{
public:
	using RuntimeException::RuntimeException;
};

class AppModule
{
public:
	virtual ~AppModule();

	virtual void onAttach(const ModuleAttachmentInfo& info) = 0;
	virtual void onDetach() = 0;
	virtual std::string getName() const = 0;
};

}// end namespace ph::editor
