#pragma once

#include "Actor/LightSource/LightSource.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"

#include <memory>

namespace ph
{
	
class IesAttenuatedSource final : public LightSource, public TCommandInterface<IesAttenuatedSource>
{
public:
	IesAttenuatedSource();
	IesAttenuatedSource(const std::shared_ptr<LightSource>& source, const Path& iesFile);

	std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;
	std::shared_ptr<Geometry> genGeometry(CookingContext& context) const override;
	std::shared_ptr<Material> genMaterial(CookingContext& context) const override;

	inline void setSource(const std::shared_ptr<LightSource>& source)
	{
		m_source = source;
	}

	inline void setIesFile(const Path& iesFile)
	{
		m_iesFile = iesFile;
	}

private:
	std::shared_ptr<LightSource> m_source;
	Path                         m_iesFile;

	static const Logger logger;

// command interface
public:
	explicit IesAttenuatedSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
