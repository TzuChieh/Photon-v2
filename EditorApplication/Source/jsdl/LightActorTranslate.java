// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-09 18:41:05.532503 

package jsdl;

public class LightActorTranslate extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(light)";
	}

	@Override
	public String getName()
	{
		return "translate";
	}

	public void setFactor(SDLVector3 data)
	{
		setInput("factor", data);
	}

}

