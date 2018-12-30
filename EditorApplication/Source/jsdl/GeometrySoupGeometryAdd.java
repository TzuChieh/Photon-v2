// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.318576 

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

