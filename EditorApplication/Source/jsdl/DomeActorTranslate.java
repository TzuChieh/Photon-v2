// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-28 15:49:23.815499 

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

