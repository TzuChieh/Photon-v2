// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-27 18:43:35.056478 

package jsdl;

public class LightActorCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(light)";
	}

	public void setLightSource(SDLLightSource data)
	{
		setInput("light-source", data);
	}

}

