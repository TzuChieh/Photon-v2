#pragma once

#include "Actor/LightSource/LightSource.h"
#include "FileIO/FileSystem/Path.h"

namespace ph
{
	
class IesAttenuatedSource final : public LightSource, public TCommandInterface<IesAttenuatedSource>
{
public:
	IesAttenuatedSource();
	IesAttenuatedSource(const Path& iesFilePath);
	virtual ~IesAttenuatedSource() override;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

private:
	Path m_iesFilePath;

// command interface
public:
	IesAttenuatedSource(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph