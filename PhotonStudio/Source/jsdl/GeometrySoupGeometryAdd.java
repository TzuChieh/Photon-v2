// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.912324 

package jsdl;

public class GeometrySoupGeometryAdd extends SDLExecutorCommand
{
	@Override
	public String getFullType()
	{
		return "geometry(geometry-soup)";
	}

	@Override
	public String getName()
	{
		return "add";
	}

	public void setGeometry(SDLGeometry data)
	{
		setInput("geometry", data);
	}

}

