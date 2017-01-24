#include "FileIO/resource_loading.h"
#include "FileIO/Tokenizer.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Texture/Texture.h"
#include "Actor/Texture/PixelTexture.h"
#include "Actor/Material/Material.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "Actor/LightSource/AreaSource.h"

#include <iostream>

namespace ph
{

std::shared_ptr<Geometry> load_geometry(const InputPacket& packet)
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
	else
	{
		std::cerr << "warning: at load_geometry(), unknown type of geometry <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<Texture> load_texture(const InputPacket& packet)
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

std::shared_ptr<Material> load_material(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "matte-opaque")
	{
		return std::make_shared<MatteOpaque>(packet);
	}
	else
	{
		std::cerr << "warning: at load_material(), unknown type of material <" << typeString << ">" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<LightSource> load_light_source(const InputPacket& packet)
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

std::unique_ptr<AModel> load_actor_model(const InputPacket& packet)
{
	return std::make_unique<AModel>(packet);
}

std::unique_ptr<ALight> load_actor_light(const InputPacket& packet)
{
	return std::make_unique<ALight>(packet);
}

}// end namespace ph