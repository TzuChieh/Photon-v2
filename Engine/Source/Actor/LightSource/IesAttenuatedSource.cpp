#include "Actor/LightSource/IesAttenuatedSource.h"
#include "FileIO/Data/IesData.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Frame/TFrame.h"

namespace ph
{

const Logger IesAttenuatedSource::logger(LogSender("IES Attenuated Source"));

IesAttenuatedSource::IesAttenuatedSource() : 
	IesAttenuatedSource(nullptr, Path())
{}

IesAttenuatedSource::IesAttenuatedSource(
	const std::shared_ptr<LightSource>& source, 
	const Path&                         iesFile) : 

	LightSource()
{
	setSource(source);
	setIesFile(iesFile);
}

IesAttenuatedSource::~IesAttenuatedSource() = default;

std::unique_ptr<Emitter> IesAttenuatedSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	if(m_source == nullptr)
	{
		logger.log(ELogLevel::WARNING_MED, 
			"no light source specified, ignoring");
		return nullptr;
	}

	const IesData iesData(m_iesFile);
	
	// TODO

	return nullptr;
}

std::shared_ptr<Geometry> IesAttenuatedSource::genGeometry(CookingContext& context) const
{
	if(m_source == nullptr)
	{
		return nullptr;
	}

	return m_source->genGeometry(context);
}

std::shared_ptr<Material> IesAttenuatedSource::genMaterial(CookingContext& context) const
{
	if(m_source == nullptr)
	{
		return nullptr;
	}

	return m_source->genMaterial(context);
}

// command interface

IesAttenuatedSource::IesAttenuatedSource(const InputPacket& packet) : 
	LightSource(packet),
	m_iesFile()
{
	m_iesFile = packet.getStringAsPath("ies-file", Path(), DataTreatment::REQUIRED());
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