// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.908324 

package jsdl;

public class PhantomModelActorScale extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(phantom-model)";
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

