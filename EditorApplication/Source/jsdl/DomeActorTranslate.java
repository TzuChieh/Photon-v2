// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.903324 

package jsdl;

public class DomeActorTranslate extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(dome)";
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

