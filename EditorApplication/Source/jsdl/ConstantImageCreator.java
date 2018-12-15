// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-09 18:41:05.543504 

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
