// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 15:13:22.658085 

package jsdl;

public class CuboidGeometryCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "geometry(cuboid)";
	}

	public void setMinVertex(SDLVector3 data)
	{
		setInput("min-vertex", data);
	}

	public void setMaxVertex(SDLVector3 data)
	{
		setInput("max-vertex", data);
	}

}

