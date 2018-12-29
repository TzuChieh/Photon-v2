package minecraft.block;

import java.util.Arrays;
import java.util.Map;

public class StateProperties
{
	private String m_properties;
	
	/**
	 * Creates a conditional from comma separated key-value pairs. 
	 * Each key-value pair is in the form key=value.
	 * @param properties Comma separated key-value pairs in the form key1=value1,key2=value2,...
	 */
	public StateProperties(String properties)
	{
		// TODO: handle possible white spaces around assignment (if the situation is possible)
		
		String[] keyValuePairs = properties.split("[,]");
		for(int i = 0; i < keyValuePairs.length; ++i)
		{
			keyValuePairs[i] = keyValuePairs[i].trim();
		}
		Arrays.sort(keyValuePairs);
		
		m_properties = String.join(",", keyValuePairs);
	}
	
	public StateProperties(Map<String, String> properties)
	{
		String[] keyValuePairs = new String[properties.size()];
		int i = 0;
		for(Map.Entry<String, String> keyValuePair : properties.entrySet())
		{
			keyValuePairs[i++] = keyValuePair.getKey() + '=' + keyValuePair.getValue();
		}
		Arrays.sort(keyValuePairs);
		
		m_properties = String.join(",", keyValuePairs);
	}
	
	public StateProperties()
	{
		m_properties = "";
	}
	
	@Override
	public boolean equals(Object obj)
	{
		if(obj == this)
            return true;
		
        if(obj == null)
            return false;
        
        if(getClass() != obj.getClass())
            return false;
        
        final StateProperties other = (StateProperties)obj;
		
		return m_properties.equals(other.m_properties);
	}

	@Override
	public int hashCode()
	{
		return m_properties.hashCode();
	}
	
	@Override
	public String toString()
	{
		return m_properties;
	}
}
