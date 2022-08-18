#pragma once

namespace ph::editor
{

// TODO: can be load/save from/to file, etc
class AppSettings final
{
public:

private:
};

class Application final
{
public:
	// TODO: CommandLineArguments for override or core settings?
	explicit Application(AppSettings settings);

private:
	AppSettings m_settings;
};

}// end namespace ph::editor
