// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 01:06:15.344929 

package jsdl;

public class LayeredSurfaceMaterialSet extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "material(layered-surface)";
	}

	@Override
	public String getName()
	{
		return "set";
	}

	public void setIndex(SDLInteger data)
	{
		setInput("index", data);
	}

	public void setRoughness(SDLReal data)
	{
		setInput("roughness", data);
	}

	public void setIorN(SDLVector3 data)
	{
		setInput("ior-n", data);
	}

	public void setIorK(SDLVector3 data)
	{
		setInput("ior-k", data);
	}

	public void setIorN(SDLReal data)
	{
		setInput("ior-n", data);
	}

	public void setIorK(SDLReal data)
	{
		setInput("ior-k", data);
	}

	public void setDepth(SDLReal data)
	{
		setInput("depth", data);
	}

	public void setG(SDLReal data)
	{
		setInput("g", data);
	}

	public void setSigmaA(SDLReal data)
	{
		setInput("sigma-a", data);
	}

	public void setSigmaS(SDLReal data)
	{
		setInput("sigma-s", data);
	}

}

