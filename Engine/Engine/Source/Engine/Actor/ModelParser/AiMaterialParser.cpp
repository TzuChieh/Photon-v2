//#include "Engine/Actor/ModelParser/AiMaterialParser.h"
//#include "Engine/Actor/Material/Material.h"
//#include "Engine/Actor/Material/MatteOpaque.h"
//#include "Engine/Core/Texture/TConstantTexture.h"
//#include "Engine/Core/Texture/TextureLoader.h"
//#include "Engine/Core/Texture/LdrRgbTexture2D.h"
//
//#include <iostream>
//
//namespace ph
//{
//
//std::shared_ptr<Material> AiMaterialParser::parseMaterial(const aiMaterial* const material, const std::string& modelDirectory)
//{
//	/*std::shared_ptr<MatteOpaque> parsedMaterial = std::make_shared<MatteOpaque>();
//	TextureLoader textureLoader;
//
//	if(material->GetTextureCount(aiTextureType_DIFFUSE) > 1)
//	{
//		std::cerr << "warning: at AiMaterialParser::parseMaterial(), there are > 1 diffuse textures, only one will be loaded" << std::endl;
//	}
//
//	if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
//	{
//		aiString textureFilename;
//
//		if(material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilename, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
//		{
//			std::string fullTextureFilename = modelDirectory + textureFilename.data;
//
//			std::shared_ptr<RgbPixelTexture> loadedTexture = std::make_shared<RgbPixelTexture>();
//			if(!textureLoader.load(fullTextureFilename, loadedTexture.getReference()))
//			{
//				std::cerr << "warning: at AiMaterialParser::parseMaterial(), texture <" << fullTextureFilename << " loading failed" << std::endl;
//			}
//
//			parsedMaterial->setAlbedo(loadedTexture);
//		}
//	}
//
//	return parsedMaterial;*/
//
//	PH_NOT_IMPLEMENTED_WARNING();
//	return nullptr;
//}
//
//}// end namespace ph