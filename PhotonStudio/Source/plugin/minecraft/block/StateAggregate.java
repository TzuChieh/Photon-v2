package plugin.minecraft.block;

import java.util.HashMap;
import java.util.Map;

public class StateAggregate
{
	private Map<String, String> m_states;
	
	/**
	 * An instance that contains no property.
	 */
	public StateAggregate()
	{
		m_states = new HashMap<>();
	}
	
	public String get(String name)
	{
		return m_states.get(name);
	}
	
	public void add(String name, String value)
	{
		m_states.put(name, value);
	}
	
	public boolean isMatching(StateAggregate otherStates)
	{
		for(Map.Entry<String, String> state : m_states.entrySet())
		{
			if(!state.getValue().equals(otherStates.get(state.getKey())))
			{
				return false;
			}
		}
		return true;
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
        
        final StateAggregate other = (StateAggregate)obj;
		
		return m_states.equals(other.m_states);
	}

	@Override
	public int hashCode()
	{
		return m_states.hashCode();
	}
	
	@Override
	public String toString()
	{
		return m_states.toString();
	}
}
