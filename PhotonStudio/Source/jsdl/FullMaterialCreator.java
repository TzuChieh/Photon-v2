// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.919325 

package jsdl;

public class FullMaterialCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "material(full)";
	}

	public void setSurface(SDLMaterial data)
	{
		setInput("surface", data);
	}

	public void setInterior(SDLMaterial data)
	{
		setInput("interior", data);
	}

	public void setExterior(SDLMaterial data)
	{
		setInput("exterior", data);
	}

}

