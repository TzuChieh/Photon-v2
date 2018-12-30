// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.394528 

package jsdl;

public class AbradedOpaqueMaterialCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "material(abraded-opaque)";
	}

	public void setType(SDLString data)
	{
		setInput("type", data);
	}

	public void setRoughness(SDLReal data)
	{
		setInput("roughness", data);
	}

	public void setF0(SDLVector3 data)
	{
		setInput("f0", data);
	}

	public void setRoughnessU(SDLReal data)
	{
		setInput("roughness-u", data);
	}

	public void setRoughnessV(SDLReal data)
	{
		setInput("roughness-v", data);
	}

}

