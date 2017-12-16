#pragma once

#include "Actor/Image/Image.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class PictureImage final : public Image, public TCommandInterface<PictureImage>
{
public:
	virtual ~PictureImage() override;

	virtual std::shared_ptr<Texture> genTexture(CookingContext& context) const = 0;
};

}// end namespace ph