// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.260611 

package jsdl;

public class TransformedInstanceActorTranslate extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(transformed-instance)";
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

