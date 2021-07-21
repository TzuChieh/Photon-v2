#pragma once

#include "DataIO/Option.h"
#include "EngineEnv/CoreSdlResource.h"
#include "DataIO/SDL/sdl_interface.h"

#include <string>
#include <memory>
#include <vector>

namespace ph { class CoreCookingContext; }
namespace ph { class SceneDescription; }

namespace ph
{

class RenderSession : public Option
{
public:
	inline RenderSession() = default;

	/*! @brief Apply settings to the context.

	Derived classes should call the base applyToContext(CoreCookingContext&) 
	in order to not miss out settings from base class.
	*/
	virtual void applyToContext(CoreCookingContext& ctx) const = 0;

	/*! @brief Gather resources for this render session.

	Similar to applyToContext(CoreCookingContext&), calling base
	gatherResources(const SceneDescription&) in derived classes is recommended.
	*/
	// TODO: should we take a context obj?
	virtual std::vector<std::shared_ptr<CoreSdlResource>> gatherResources(const SceneDescription& scene) const = 0;

private:
	std::string m_sessionName;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<RenderSession>)
	{
		ClassType clazz("render-session");
		clazz.description("Settings for how to perform a render operation.");
		clazz.baseOn<Option>();

		TSdlString<OwnerType> sessionName("session-name", &OwnerType::m_sessionName);
		sessionName.description("Name of this render session.");
		sessionName.defaultTo("Render Session");
		sessionName.optional();
		clazz.addField(sessionName);

		return clazz;
	}
};

// In-header Implementations:

inline void RenderSession::applyToContext(CoreCookingContext& ctx) const
{}

inline std::vector<std::shared_ptr<CoreSdlResource>> RenderSession::gatherResources(const SceneDescription& scene) const
{}

}// end namespace ph
