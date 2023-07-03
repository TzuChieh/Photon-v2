#pragma once

#include "Actor/Light/ALight.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "DataIO/FileSystem/Path.h"
#include "SDL/sdl_interface.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

class AIesAttenuatedLight : public ALight
{
public:
	PreCookReport preCook(const CookingContext& ctx) const override;
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override;

	void setSource(const std::shared_ptr<ALight>& source);
	void setIesFile(const Path& iesFile);

private:
	TransientVisualElement getSourceVisualElement(const CookingContext& ctx) const;
	std::shared_ptr<TTexture<math::Spectrum>> loadAttenuationTexture() const;

	std::shared_ptr<ALight> m_source;
	ResourceIdentifier m_iesFile;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AIesAttenuatedLight>)
	{
		ClassType clazz("ies-attenuated-light");
		clazz.docName("IES-Attenuated Light Actor");
		clazz.description(
			"Attenuating energy emitting strength of a light with an IES profile.");
		clazz.baseOn<ALight>();

		TSdlReference<ALight, OwnerType> source("source", &OwnerType::m_source);
		source.description("The light source that will be attenuated.");
		source.optional();
		clazz.addField(source);

		TSdlResourceIdentifier<OwnerType> iesFile("ies-file", &OwnerType::m_iesFile);
		iesFile.description("The IES file.");
		iesFile.required();
		clazz.addField(iesFile);

		return clazz;
	}
};

}// end namespace ph
