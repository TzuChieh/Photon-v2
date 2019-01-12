package appModel;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Properties;

import appModel.event.SettingEvent;
import appModel.event.SettingListener;

public abstract class SettingGroup
{
	// TODO: consider making listener listening to specific setting
	
	private Map<String, String>   m_settings;
	private List<SettingListener> m_settingListeners;
	
	protected SettingGroup()
	{
		m_settings         = new HashMap<>();
		m_settingListeners = new ArrayList<>();
	}
	
	public abstract void setToDefaults();
	
	public String get(String settingName)
	{
		return m_settings.get(settingName);
	}
	
	public void set(final String settingName, final String newSettingValue)
	{
		final String oldSettingValue = m_settings.get(settingName);
		if(Objects.equals(oldSettingValue, newSettingValue))
		{
			return;
		}
		
		m_settings.put(settingName, newSettingValue);
		
		for(SettingListener listener : m_settingListeners)
		{
			SettingEvent event    = new SettingEvent();
			event.source          = this;
			event.settingName     = settingName;
			event.oldSettingValue = oldSettingValue;
			event.newSettingValue = newSettingValue;
			
			listener.onSettingChanged(event);
		}
	}
	
	public void addSettingListener(SettingListener listener)
	{
		m_settingListeners.add(listener);
	}
	
	public void removeSettingListener(SettingListener listener)
	{
		while(m_settingListeners.remove(listener));
	}
	
	public void saveToFile(String fullFilename)
	{
		OutputStream ostream = null;

		try
		{
			Properties props = new Properties();
			for(Map.Entry<String, String> mapEntry : m_settings.entrySet())
			{
				props.setProperty(mapEntry.getKey(), mapEntry.getValue());
			}
			
			ostream = new FileOutputStream(new File(fullFilename));
			props.store(ostream, null);
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		finally
		{
			if(ostream != null)
			{
				try
				{
					ostream.close();
				}
				catch(IOException e)
				{
					e.printStackTrace();
				}
			}
		}
	}
	
	public void loadFromFile(String fullFilename)
	{
    	InputStream istream = null;

    	try
    	{
    		istream = new FileInputStream(new File(fullFilename));
    		
    		Properties props = new Properties();
    		props.load(istream);
			for(final String keyName : props.stringPropertyNames())
			{
				m_settings.put(keyName, props.getProperty(keyName));
			}
    	}
    	catch(IOException e)
    	{
    		e.printStackTrace();
        }
    	finally
    	{
        	if(istream != null)
        	{
        		try
        		{
        			istream.close();
        		}
        		catch(IOException e)
        		{
        			e.printStackTrace();
        		}
        	}
        }
	}
}
