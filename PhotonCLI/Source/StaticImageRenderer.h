#pragma once

#include "ProcessedArguments.h"

#include <ph_c_core.h>

#include <string>

namespace ph::cli
{

class StaticImageRenderer
{
public:
	explicit StaticImageRenderer(const ProcessedArguments& args);
	virtual ~StaticImageRenderer();

	virtual void render() = 0;

	void setSceneFilePath(const std::string& path);
	void setImageOutputPath(const std::string& path);

protected:
	PhUInt64 getEngine() const;
	const ProcessedArguments& getArgs() const;
	bool loadCommandsFromSceneFile() const;

private:
	PhUInt64 m_engineId;
	ProcessedArguments m_args;
};

inline PhUInt64 StaticImageRenderer::getEngine() const
{
	return m_engineId;
}

inline const ProcessedArguments& StaticImageRenderer::getArgs() const
{
	return m_args;
}

}// end namespace ph::cli
