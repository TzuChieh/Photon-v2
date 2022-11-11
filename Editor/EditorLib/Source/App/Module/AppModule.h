#pragma once

#include <string>

namespace ph::editor
{

class Application;
class ModuleAttachmentInfo;

class AppModule
{
public:
	virtual ~AppModule();

	virtual void attach(Application& app) = 0;
	virtual void onAttach(const ModuleAttachmentInfo& info) = 0;
	virtual void onDetach() = 0;
	virtual std::string getName() const = 0;
};

}// end namespace ph::editor
