#include "Actor/LightSource/IesAttenuatedSource.h"
#include "FileIO/Data/IesData.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/PrimitiveChannel.h"

namespace ph
{

IesAttenuatedSource::IesAttenuatedSource() : 
	IesAttenuatedSource(Path())
{}

IesAttenuatedSource::IesAttenuatedSource(const Path& iesFilePath) : 
	LightSource(),
	m_iesFilePath(iesFilePath)
{}

IesAttenuatedSource::~IesAttenuatedSource() = default;

CookedUnit IesAttenuatedSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	const IesData iesData(m_iesFilePath);
	
}

// command interface

IesAttenuatedSource::IesAttenuatedSource(const InputPacket& packet) : 
	LightSource(packet)
{
	// TODO
}

SdlTypeInfo IesAttenuatedSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "ies-attenuated");
}

void IesAttenuatedSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<IesAttenuatedSource>(packet);
	}));
}

}// end namespace ph