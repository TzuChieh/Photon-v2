// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-09 18:41:05.554504 

package jsdl;

public class PmRendererCreator extends SDLCoreCommand
{
	@Override
	public String getFullType()
	{
		return "renderer(pm)";
	}

	public void setMode(SDLString data)
	{
		setInput("mode", data);
	}

	public void setNumPhotons(SDLInteger data)
	{
		setInput("num-photons", data);
	}

	public void setRadius(SDLReal data)
	{
		setInput("radius", data);
	}

	public void setNumPasses(SDLInteger data)
	{
		setInput("num-passes", data);
	}

	public void setNumSamplesPerPixel(SDLInteger data)
	{
		setInput("num-samples-per-pixel", data);
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

