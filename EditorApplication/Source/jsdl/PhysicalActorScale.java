// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 01:06:15.334929 

package jsdl;

public class PhysicalActorScale extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(physical)";
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

