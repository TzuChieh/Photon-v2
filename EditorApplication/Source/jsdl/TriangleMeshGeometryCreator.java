// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.914324 

package jsdl;

public class TriangleMeshGeometryCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "geometry(triangle-mesh)";
	}

	public void setPositions(SDLVector3Array data)
	{
		setInput("positions", data);
	}

	public void setTextureCoordinates(SDLVector3Array data)
	{
		setInput("texture-coordinates", data);
	}

	public void setNormals(SDLVector3Array data)
	{
		setInput("normals", data);
	}

}

