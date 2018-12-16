// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 01:06:15.338929 

package jsdl;

public class RealMathImageCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "image(real-math)";
	}

	public void setMathOp(SDLString data)
	{
		setInput("math-op", data);
	}

	public void setValue(SDLReal data)
	{
		setInput("value", data);
	}

	public void setOperand(SDLImage data)
	{
		setInput("operand", data);
	}

}

