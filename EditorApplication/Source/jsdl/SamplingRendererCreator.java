// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2019-01-01 17:46:28.921325 

package jsdl;

public class SamplingRendererCreator extends SDLCoreCommand
{
	@Override
	public String getFullType()
	{
		return "renderer(sampling)";
	}

	public void setFilterName(SDLString data)
	{
		setInput("filter-name", data);
	}

	public void setEstimator(SDLString data)
	{
		setInput("estimator", data);
	}

	public void setLightEnergyTag(SDLString data)
	{
		setInput("light-energy-tag", data);
	}

	public void setNormalTag(SDLString data)
	{
		setInput("normal-tag", data);
	}

	public void setWidth(SDLInteger data)
	{
		setInput("width", data);
	}

	public void setHeight(SDLInteger data)
	{
		setInput("height", data);
	}

	public void setRectX(SDLInteger data)
	{
		setInput("rect-x", data);
	}

	public void setRectY(SDLInteger data)
	{
		setInput("rect-y", data);
	}

	public void setRectW(SDLInteger data)
	{
		setInput("rect-w", data);
	}

	public void setRectH(SDLInteger data)
	{
		setInput("rect-h", data);
	}

}

