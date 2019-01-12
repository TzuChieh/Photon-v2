// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.915324 

package jsdl;

public class ConstantImageCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "image(constant)";
	}

	public void setValueType(SDLString data)
	{
		setInput("value-type", data);
	}

	public void setValue(SDLReal data)
	{
		setInput("value", data);
	}

	public void setValue(SDLVector3 data)
	{
		setInput("value", data);
	}

}

