// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.911324 

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

	public void setPxFaceUv(SDLQuaternion data)
	{
		setInput("px-face-uv", data);
	}

	public void setNxFaceUv(SDLQuaternion data)
	{
		setInput("nx-face-uv", data);
	}

	public void setPzFaceUv(SDLQuaternion data)
	{
		setInput("pz-face-uv", data);
	}

	public void setNzFaceUv(SDLQuaternion data)
	{
		setInput("nz-face-uv", data);
	}

	public void setPyFaceUv(SDLQuaternion data)
	{
		setInput("py-face-uv", data);
	}

	public void setNyFaceUv(SDLQuaternion data)
	{
		setInput("ny-face-uv", data);
	}

}

