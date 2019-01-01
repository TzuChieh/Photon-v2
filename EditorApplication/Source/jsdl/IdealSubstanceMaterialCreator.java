// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.919325 

package jsdl;

public class IdealSubstanceMaterialCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "material(ideal-substance)";
	}

	public void setType(SDLString data)
	{
		setInput("type", data);
	}

	public void setIorOuter(SDLReal data)
	{
		setInput("ior-outer", data);
	}

	public void setIorInner(SDLReal data)
	{
		setInput("ior-inner", data);
	}

	public void setF0Rgb(SDLVector3 data)
	{
		setInput("f0-rgb", data);
	}

	public void setReflectionScale(SDLVector3 data)
	{
		setInput("reflection-scale", data);
	}

	public void setTransmissionScale(SDLVector3 data)
	{
		setInput("transmission-scale", data);
	}

}

