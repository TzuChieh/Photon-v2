// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.227631 

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

