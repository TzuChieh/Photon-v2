// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-28 15:49:23.819499 

package jsdl;

public class ModelActorScale extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(model)";
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

