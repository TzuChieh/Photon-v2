// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.915324 

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

