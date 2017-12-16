#pragma once

#include "FileIO/SDL/TCommandInterface.h"

#include <memory>

namespace ph
{

class Texture;
class CookingContext;

class Image : public TCommandInterface<Image>
{
public:
	virtual ~Image() = 0;

	virtual std::shared_ptr<Texture> genTexture(CookingContext& context) const = 0;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph