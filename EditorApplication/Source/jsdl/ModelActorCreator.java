// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-27 18:43:35.057478 

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

