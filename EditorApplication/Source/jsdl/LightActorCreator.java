// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.176655 

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

