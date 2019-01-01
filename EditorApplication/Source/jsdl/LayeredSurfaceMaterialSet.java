// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.920325 

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

