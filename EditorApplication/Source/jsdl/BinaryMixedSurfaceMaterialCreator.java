// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-17 15:13:22.676086 

package jsdl;

public class BinaryMixedSurfaceMaterialCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "material(binary-mixed-surface)";
	}

	public void setMode(SDLString data)
	{
		setInput("mode", data);
	}

	public void setFactor(SDLReal data)
	{
		setInput("factor", data);
	}

	public void setMaterial0(SDLMaterial data)
	{
		setInput("material-0", data);
	}

	public void setMaterial1(SDLMaterial data)
	{
		setInput("material-1", data);
	}

}

