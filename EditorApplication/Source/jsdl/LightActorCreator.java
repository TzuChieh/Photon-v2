// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.904324 

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

