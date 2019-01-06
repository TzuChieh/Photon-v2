#pragma once

#include "Actor/Actor.h"
#include "Utility/INoncopyable.h"
#include "Actor/CookedUnit.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

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

	void addPhantom(const std::string& name, CookedUnit phantom);
	const CookedUnit* getPhantom(const std::string& name) const;

	std::vector<std::unique_ptr<Actor>> claimChildActors();

	const VisualWorldInfo* getVisualWorldInfo() const;

protected:
	void setVisualWorldInfo(const VisualWorldInfo* info);

private:
	std::vector<std::unique_ptr<Actor>>         m_childActors;
	std::unordered_map<std::string, CookedUnit> m_phantoms;
	const VisualWorldInfo*                      m_visualWorldInfo;
};

// In-header Implementations:

inline const VisualWorldInfo* CookingContext::getVisualWorldInfo() const
{
	return m_visualWorldInfo;
}

}// end namespace ph