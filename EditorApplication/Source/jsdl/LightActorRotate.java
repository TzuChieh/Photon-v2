// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.189647 

package jsdl;

public class LightActorRotate extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(light)";
	}

	@Override
	public String getName()
	{
		return "rotate";
	}

	public void setAxis(SDLVector3 data)
	{
		setInput("axis", data);
	}

	public void setDegree(SDLReal data)
	{
		setInput("degree", data);
	}

	public void setFactor(SDLQuaternion data)
	{
		setInput("factor", data);
	}

}

