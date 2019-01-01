// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.921325 

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

	public void setRotation(SDLQuaternion data)
	{
		setInput("rotation", data);
	}

	public void setDirection(SDLVector3 data)
	{
		setInput("direction", data);
	}

	public void setUpAxis(SDLVector3 data)
	{
		setInput("up-axis", data);
	}

	public void setYawDegrees(SDLReal data)
	{
		setInput("yaw-degrees", data);
	}

	public void setPitchDegrees(SDLReal data)
	{
		setInput("pitch-degrees", data);
	}

}

