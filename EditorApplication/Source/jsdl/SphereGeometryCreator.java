// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.913324 

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

