package jsdl;

import java.util.ArrayList;

public class SDLVector3Array implements SDLData
{
	private ArrayList<Float> m_array;

	public SDLVector3Array()
	{
		m_array = new ArrayList<>();
	}
	
	@Override
	public String getType()
	{
		return "vector3-array";
	}

	@Override
	public void generateData(StringBuilder out_sdlBuffer)
	{
		assert(m_array.size() % 3 == 0);
		
		out_sdlBuffer.append('{');
		for(int i = 0; i < m_array.size(); i += 3)
		{
			float x = m_array.get(i);
			float y = m_array.get(i + 1);
			float z = m_array.get(i + 2);
			out_sdlBuffer.append('\"');
			out_sdlBuffer.append(Float.toString(x) + ' ');
			out_sdlBuffer.append(Float.toString(y) + ' ');
			out_sdlBuffer.append(Float.toString(z));
			out_sdlBuffer.append("\" ");
		}
		out_sdlBuffer.append('}');
	}
	
	public void add(float x, float y, float z)
	{
		m_array.add(x);
		m_array.add(y);
		m_array.add(z);
	}
}
