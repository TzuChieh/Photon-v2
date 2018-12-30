// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.388532 

package jsdl;

public class SphereLightSourceCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "light-source(sphere)";
	}

	public void setRadius(SDLReal data)
	{
		setInput("radius", data);
	}

	public void setLinearSrgb(SDLVector3 data)
	{
		setInput("linear-srgb", data);
	}

	public void setWatts(SDLReal data)
	{
		setInput("watts", data);
	}

}

