#pragma once

#include "RenderCore/GHI.h"

namespace ph::editor
{

class GlfwGladOpenglGHI : public GHI
{
public:
	GlfwGladOpenglGHI();
	~GlfwGladOpenglGHI() override;

private:
};

}// end namespace ph::editor
