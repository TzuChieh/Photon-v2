// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.916324 

package jsdl;

public class ModelLightSourceCreator extends SDLCreatorCommand
{
	@Override
	public String getFullType()
	{
		return "light-source(model)";
	}

	public void setGeometry(SDLGeometry data)
	{
		setInput("geometry", data);
	}

	public void setMaterial(SDLMaterial data)
	{
		setInput("material", data);
	}

	public void setEmittedRadiance(SDLImage data)
	{
		setInput("emitted-radiance", data);
	}

	public void setEmittedRadiance(SDLVector3 data)
	{
		setInput("emitted-radiance", data);
	}

	public void setEmitMode(SDLString data)
	{
		setInput("emit-mode", data);
	}

}

