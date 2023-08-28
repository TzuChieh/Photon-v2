#pragma once

namespace ph::editor
{

class GHI;
class GraphicsObjectManager;

class GraphicsContext
{
public:
	virtual ~GraphicsContext();

	virtual GHI& getGHI() = 0;
	virtual GraphicsObjectManager& getObjectManager() = 0;
};

}// end namespace ph::editor
