// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-09 18:41:05.541504 

package jsdl;

public class SphereGeometryCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "geometry(sphere)";
	}

	public void setRadius(SDLReal data)
	{
		setInput("radius", data);
	}

}

