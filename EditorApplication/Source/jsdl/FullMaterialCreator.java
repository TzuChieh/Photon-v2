// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-09 18:41:05.549504 

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
