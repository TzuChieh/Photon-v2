#include "FileIO/ResourceLoader.h"
#include "FileIO/InputPacket.h"
#include "FileIO/Tokenizer.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Texture/Texture.h"
#include "Actor/Texture/PixelTexture.h"
#include "Actor/Material/Material.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Material/AbradedOpaque.h"
#include "Actor/Material/AbradedTranslucent.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "Actor/LightSource/AreaSource.h"
#include "Core/Camera/PinholeCamera.h"
#include "Core/Filmic/Film.h"
#include "Core/SampleGenerator/PixelJitterSampleGenerator.h"
#include "Core/Integrator/BackwardPathIntegrator.h"
#include "Core/Integrator/BackwardLightIntegrator.h"
#include "Core/Integrator/BackwardMisIntegrator.h"
#include "Core/Integrator/LightTracingIntegrator.h"
#include "Core/Integrator/NormalBufferIntegrator.h"

#include <iostream>

namespace ph
{

std::unique_ptr<Camera> ResourceLoader::loadCamera(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "pinhole")
	{
		return std::make_unique<PinholeCamera>(packet);
	}
	else
	{
		std::cerr << "warning: at load_camera(), unknown type of camera <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::unique_ptr<Film> ResourceLoader::loadFilm(const InputPacket& packet)
{
	//return std::make_unique<Film>(packet);
	std::cerr << "ResourceLoader::loadFilm(), null" << std::endl;
	return nullptr;
}

std::unique_ptr<Integrator> ResourceLoader::loadIntegrator(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "backward-path")
	{
		return std::make_unique<BackwardPathIntegrator>(packet);
	}
	else if(typeString == "backward-light")
	{
		return std::make_unique<BackwardLightIntegrator>(packet);
	}
	else if(typeString == "backward-mis")
	{
		return std::make_unique<BackwardMisIntegrator>(packet);
	}
	else if(typeString == "light-tracing")
	{
		return std::make_unique<LightTracingIntegrator>(packet);
	}
	else if(typeString == "surface-normal")
	{
		return std::make_unique<NormalBufferIntegrator>(packet);
	}
	else
	{
		std::cerr << "warning: at load_integrator(), unknown type of integrator <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::unique_ptr<SampleGenerator> ResourceLoader::loadSampleGenerator(const InputPacket& packet)
{
	return std::make_unique<PixelJitterSampleGenerator>(packet);
}

std::shared_ptr<Geometry> ResourceLoader::loadGeometry(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "sphere")
	{
		return std::make_shared<GSphere>(packet);
	}
	else if(typeString == "rectangle")
	{
		return std::make_shared<GRectangle>(packet);
	}
	else if(typeString == "triangle-mesh")
	{
		return std::make_shared<GTriangleMesh>(packet);
	}
	else
	{
		std::cerr << "warning: at load_geometry(), unknown type of geometry <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<Texture> ResourceLoader::loadTexture(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "pixel")
	{
		return std::make_shared<PixelTexture>(packet);
	}
	else
	{
		std::cerr << "warning: at load_texture(), unknown type of texture <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<Material> ResourceLoader::loadMaterial(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "matte-opaque")
	{
		return std::make_shared<MatteOpaque>(packet);
	}
	else if(typeString == "abraded-opaque")
	{
		return std::make_shared<AbradedOpaque>(packet);
	}
	else if(typeString == "abraded-translucent")
	{
		return std::make_shared<AbradedTranslucent>(packet);
	}
	else
	{
		std::cerr << "warning: at load_material(), unknown type of material <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<LightSource> ResourceLoader::loadLightSource(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "area")
	{
		return std::make_shared<AreaSource>(packet);
	}
	else
	{
		std::cerr << "warning: at load_light_source(), unknown type of light source <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::unique_ptr<AModel> ResourceLoader::loadActorModel(const InputPacket& packet)
{
	return std::make_unique<AModel>(packet);
}

std::unique_ptr<ALight> ResourceLoader::loadActorLight(const InputPacket& packet)
{
	return std::make_unique<ALight>(packet);
}

}// end namespace ph