package appModel;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Objects;

public class Setting
{
	private String                m_name;
	private String                m_value;
	private List<SettingListener> m_listeners;
	
	public Setting(String name)
	{
		m_name      = name;
		m_value     = "";
		m_listeners = null;
	}
	
	public String getName()
	{
		return m_name;
	}
	
	public String getValue()
	{
		return m_value;
	}
	
	public void setValue(final String newValue)
	{
		final String oldValue = m_value;
		if(Objects.equals(oldValue, newValue))
		{
			return;
		}
		
		m_value = newValue;
		
		if(m_listeners != null)
		{
			for(SettingListener listener : m_listeners)
			{
				listener.onChanged(m_name, oldValue, newValue);
			}
		}
	}
	
	public void addListener(SettingListener listener)
	{
		addListener(listener, true);
	}
	
	public void addListener(SettingListener listener, boolean notifyAfterAdding)
	{
		if(m_listeners == null)
		{
			m_listeners = new ArrayList<>();
		}
		m_listeners.add(listener);
		
		if(notifyAfterAdding)
		{
			listener.onChanged(m_name, m_value, m_value);
		}
	}
	
	public void removeListener(SettingListener listener)
	{
		if(m_listeners != null)
		{
			Iterator<SettingListener> iterator = m_listeners.iterator();
			while(iterator.hasNext())
			{
				if(iterator.next() == listener)
				{
					iterator.remove();
				}
			}
		}
	}
}
