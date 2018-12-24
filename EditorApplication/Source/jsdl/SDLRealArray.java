package jsdl;

import java.util.ArrayList;

public class SDLRealArray implements SDLData
{
	private ArrayList<Float> m_array;

	public SDLRealArray()
	{
		m_array = new ArrayList<>();
	}
	
	@Override
	public String getType()
	{
		return "real-array";
	}

	@Override
	public void generateData(StringBuilder out_sdlBuffer)
	{
		out_sdlBuffer.append('{');
		for(float real : m_array)
		{
			out_sdlBuffer.append(Float.toString(real));
			out_sdlBuffer.append(' ');
		}
		out_sdlBuffer.append('}');
	}
	
	public void add(float real)
	{
		m_array.add(real);
	}
	
	public void add(float[] reals)
	{
		for(float real : reals)
		{
			add(real);
		}
	}
}
