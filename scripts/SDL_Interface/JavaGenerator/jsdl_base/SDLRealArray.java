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
	public String generateData()
	{
		StringBuilder fragments = new StringBuilder();
		fragments.append('{');
		for(float real : m_array)
		{
			fragments.append(Float.toString(real) + ' ');
		}
		fragments.append('}');
		return fragments.toString();
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
