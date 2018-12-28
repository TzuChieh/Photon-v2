// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-28 11:02:25.071208 

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

