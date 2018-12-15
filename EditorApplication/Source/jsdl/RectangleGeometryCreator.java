// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-09 18:41:05.541504 

package jsdl;

public class RectangleGeometryCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "geometry(rectangle)";
	}

	public void setWidth(SDLReal data)
	{
		setInput("width", data);
	}

	public void setHeight(SDLReal data)
	{
		setInput("height", data);
	}

	public void setTexcoordScale(SDLReal data)
	{
		setInput("texcoord-scale", data);
	}

}

