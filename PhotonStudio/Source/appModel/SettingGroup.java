package appModel;

import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

public class SettingGroup
{
	private Map<String, Setting> m_settings;
	
	public SettingGroup()
	{
		m_settings = new HashMap<>();
	}
	
	public String get(final String name)
	{
		return getSetting(name).getValue();
	}
	
	public Setting getSetting(final String name)
	{
		return m_settings.get(name);
	}
	
	public boolean has(final String name)
	{
		return m_settings.containsKey(name);
	}
	
	public void set(final String name, final String value)
	{
		getSetting(name).setValue(value);
	}
	
	public void add(final String name, final String value)
	{
		assert(!has(name));
		
		Setting setting = new Setting(name);
		setting.setValue(value);
		m_settings.put(name, setting);
	}
	
	public void add(SettingGroup other)
	{
		for(Setting setting : other.m_settings.values())
		{
			add(setting.getName(), setting.getValue());
		}
	}
	
	public void removeSetting(final String name)
	{
		m_settings.remove(name);
	}
	
	public void save(final Path path)
	{
		try(OutputStream ostream = Files.newOutputStream(path))
		{
			Properties props = new Properties();
			for(Setting setting : m_settings.values())
			{
				props.setProperty(setting.getName(), setting.getValue());
			}
			props.store(ostream, null);
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public void load(final Path path)
	{
    	try(InputStream istream = Files.newInputStream(path))
    	{
    		Properties props = new Properties();
    		props.load(istream);
			for(String keyName : props.stringPropertyNames())
			{
				if(has(keyName))
				{
					set(keyName, props.getProperty(keyName));
				}
				else
				{
					add(keyName, props.getProperty(keyName));
				}
			}
    	}
    	catch(Exception e)
    	{
    		e.printStackTrace();
        }
	}
}
