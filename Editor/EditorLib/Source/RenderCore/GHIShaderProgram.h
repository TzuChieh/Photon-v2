#pragma once

namespace ph::editor
{

class GHIShaderProgram
{
public:
	virtual ~GHIShaderProgram();

	virtual void bind() = 0;
};

}// end namespace ph::editor
