#pragma once

#include "Actor/Actor.h"

#include <vector>
#include <memory>

namespace ph
{

class CookingContext final
{
public:
	// TODO: we can assign child actors special attributes such as
	// deferred cooking, which opens the possibility of calculating
	// full scene bound before cooking (or their parent actor)
	void addChildActor(std::unique_ptr<Actor> actor);

	std::vector<std::unique_ptr<Actor>> claimChildActors();

private:
	std::vector<std::unique_ptr<Actor>> m_childActors;
};

}// end namespace ph