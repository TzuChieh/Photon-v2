// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.913324 

package jsdl;

public class GeometrySoupGeometryAddTransformed extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "geometry(geometry-soup)";
	}

	@Override
	public String getName()
	{
		return "add-transformed";
	}

	public void setGeometry(SDLGeometry data)
	{
		setInput("geometry", data);
	}

	public void setTranslation(SDLVector3 data)
	{
		setInput("translation", data);
	}

	public void setRotationAxis(SDLVector3 data)
	{
		setInput("rotation-axis", data);
	}

	public void setRotationDegrees(SDLReal data)
	{
		setInput("rotation-degrees", data);
	}

	public void setScale(SDLVector3 data)
	{
		setInput("scale", data);
	}

}

