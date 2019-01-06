// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.920325 

package jsdl;

public class MatteOpaqueMaterialCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "material(matte-opaque)";
	}

	public void setAlbedo(SDLReal data)
	{
		setInput("albedo", data);
	}

	public void setAlbedo(SDLVector3 data)
	{
		setInput("albedo", data);
	}

	public void setAlbedo(SDLImage data)
	{
		setInput("albedo", data);
	}

}

