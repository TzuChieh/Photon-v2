// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-28 15:49:23.817499 

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

