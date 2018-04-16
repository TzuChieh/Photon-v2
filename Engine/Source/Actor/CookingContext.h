#pragma once

#include "Actor/Actor.h"
#include "Utility/INoncopyable.h"

#include <vector>
#include <memory>

namespace ph
{

class VisualWorldInfo;

class CookingContext final : public INoncopyable
{
	friend class VisualWorld;

public:
	CookingContext();

	// TODO: we can assign child actors special attributes such as
	// deferred cooking, which opens the possibility of calculating
	// full scene bound before cooking (or their parent actor)
	void addChildActor(std::unique_ptr<Actor> actor);

	std::vector<std::unique_ptr<Actor>> claimChildActors();

	inline const VisualWorldInfo* getVisualWorldInfo() const
	{
		return m_visualWorldInfo;
	}

protected:
	void setVisualWorldInfo(const VisualWorldInfo* info);

private:
	std::vector<std::unique_ptr<Actor>> m_childActors;
	const VisualWorldInfo*              m_visualWorldInfo;
};

}// end namespace ph