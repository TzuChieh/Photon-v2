package util.minecraft;

import java.util.HashMap;
import java.util.Map;

public class JSONObject
{
	private Map<String, Object> m_nameToValues;
	
	public JSONObject()
	{
		m_nameToValues = new HashMap<>();
	}
	
	public boolean getBoolean(String name)
	{
		return Boolean.valueOf(getString(name));
	}
	
	public double getNumber(String name)
	{
		return Double.valueOf(getString(name));
	}
	
	public String getString(String name)
	{
		return (String)get(name);
	}
	
	public JSONObject getObject(String name)
	{
		return (JSONObject)get(name);
	}
	
	public JSONArray getArray(String name)
	{
		return (JSONArray)get(name);
	}
	
	public Object get(String name)
	{
		if(!has(name))
		{
			System.err.println("data " + name + " do not exist");
		}
		
		return m_nameToValues.get(name);
	}
	
	public void add(String name, Object value)
	{
		if(has(name))
		{
			System.err.println("data " + name + " exists, overwriting");
		}
		
		m_nameToValues.put(name, value);
	}
	
	public boolean has(String name)
	{
		return m_nameToValues.containsKey(name);
	}
}
