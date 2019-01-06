package minecraft;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class JSONArray implements Iterable<Object>
{
	private List<Object> m_values;
	
	public JSONArray()
	{
		m_values = new ArrayList<>();
	}
	
	public boolean getBoolean(int index)
	{
		return Boolean.valueOf(getString(index));
	}
	
	public double getNumber(int index)
	{
		return Double.valueOf(getString(index));
	}
	
	public String getString(int index)
	{
		return (String)get(index);
	}
	
	public JSONObject getObject(int index)
	{
		return (JSONObject)get(index);
	}
	
	public JSONArray getArray(int index)
	{
		return (JSONArray)get(index);
	}
	
	public Object get(int index)
	{
		return (Object)m_values.get(index);
	}
	
	public void add(Object value)
	{
		m_values.add(value);
	}
	
	public int numValues()
	{
		return m_values.size();
	}

	@Override
	public Iterator<Object> iterator()
	{
		return m_values.iterator();
	}
	
	@Override
	public String toString()
	{
		String result = "JSON Array: [\n";
		for(int i = 0; i < m_values.size(); ++i)
		{
			result += i + ": <" + m_values.get(i).toString() + ">\n";
		}
		return result + "]";
	}
}
