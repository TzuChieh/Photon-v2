// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 01:06:15.340929 

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

