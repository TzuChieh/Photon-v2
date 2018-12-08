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
	public String generateData()
	{
		assert(m_array.size() % 3 == 0);
		
		StringBuilder fragments = new StringBuilder();
		fragments.append('{');
		for(int i = 0; i < m_array.size(); i += 3)
		{
			float x = m_array.get(i);
			float y = m_array.get(i + 1);
			float z = m_array.get(i + 2);
			fragments.append('\"');
			fragments.append(Float.toString(x) + ' ');
			fragments.append(Float.toString(y) + ' ');
			fragments.append(Float.toString(z));
			fragments.append("\" ");
		}
		fragments.append('}');
		return fragments.toString();
	}
	
	public void add(float x, float y, float z)
	{
		m_array.add(x);
		m_array.add(y);
		m_array.add(z);
	}
}
