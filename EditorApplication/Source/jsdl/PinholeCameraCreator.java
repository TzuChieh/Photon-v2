// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 15:13:22.686087 

package jsdl;

public class PinholeCameraCreator extends SDLCoreCommand
{
	@Override
	public String getFullType()
	{
		return "camera(pinhole)";
	}

	public void setFovDegree(SDLReal data)
	{
		setInput("fov-degree", data);
	}

	public void setFilmWidthMm(SDLReal data)
	{
		setInput("film-width-mm", data);
	}

	public void setFilmOffsetMm(SDLReal data)
	{
		setInput("film-offset-mm", data);
	}

	public void setPosition(SDLVector3 data)
	{
		setInput("position", data);
	}

	public void setDirection(SDLVector3 data)
	{
		setInput("direction", data);
	}

	public void setUpAxis(SDLVector3 data)
	{
		setInput("up-axis", data);
	}

}

