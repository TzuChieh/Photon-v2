// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.233629 

package jsdl;

public class PhantomModelActorTranslate extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(phantom-model)";
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

