// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 01:06:15.331928 

package jsdl;

public class ModelActorCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(model)";
	}

	public void setGeometry(SDLGeometry data)
	{
		setInput("geometry", data);
	}

	public void setMaterial(SDLMaterial data)
	{
		setInput("material", data);
	}

}

