// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.907324 

package jsdl;

public class PhantomModelActorCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "actor(phantom-model)";
	}

	public void setName(SDLString data)
	{
		setInput("name", data);
	}

	public void setGeometry(SDLGeometry data)
	{
		setInput("geometry", data);
	}

	public void setMaterial(SDLMaterial data)
	{
		setInput("material", data);
	}

}

