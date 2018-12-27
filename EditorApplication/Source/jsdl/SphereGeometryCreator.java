// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-27 18:43:35.061479 

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

