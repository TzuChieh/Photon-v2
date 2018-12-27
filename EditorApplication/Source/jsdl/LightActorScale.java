// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-27 18:43:35.057478 

package jsdl;

public class LightActorScale extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(light)";
	}

	@Override
	public String getName()
	{
		return "scale";
	}

	public void setFactor(SDLVector3 data)
	{
		setInput("factor", data);
	}

}

